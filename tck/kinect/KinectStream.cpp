#include "tck/kinect/KinectStream.hpp"
#include <iostream>

/// <summary>
/// Constructor
/// </summary>
/// <param name="pNuiSensor">The pointer to Nui sensor device instance</param>
KinectStream::KinectStream(INuiSensor* pNuiSensor) :
    m_pNuiSensor(pNuiSensor),
    //m_pStreamViewer(nullptr),
    m_hStreamHandle(INVALID_HANDLE_VALUE),
    m_paused(false)
{
    if(m_pNuiSensor)
    {
        m_pNuiSensor->AddRef();
    }

    m_hFrameReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
}

/// <summary>
/// Destructor
/// </summary>
KinectStream::~KinectStream()
{
    /*if(m_pStreamViewer)
    {
        // Clear reference to image buffer in stream viewer
        m_pStreamViewer->SetImage(nullptr);
    }*/

    CloseHandle(m_hFrameReadyEvent);
    SafeRelease(m_pNuiSensor);
}

/// <summary>
/// Get stream frame ready event handle
/// </summary>
/// <returns>Handle to event</returns>
HANDLE KinectStream::GetFrameReadyEvent()
{
    return m_hFrameReadyEvent;
}

/// <summary>
/// Pause the stream
/// </summary>
/// <param name="pause">Pause or resume the stream</param>
void KinectStream::PauseStream(bool pause)
{
    m_paused = pause;

    // And meanwhile pause the skeleton
    //m_pStreamViewer->PauseSkeleton(pause);
}

/// <summary>
/// Attach viewer object to stream object
/// </summary>
/// <param name="pStreamViewer">The pointer to viewer object to attach</param>
/// <returns>Previously attached viewer object. If none, returns nullptr</returns>
/*KinectStreamViewer* KinectStream::SetStreamViewer(KinectStreamViewer * pStreamViewer)
{
    KinectStreamViewer* pOldViewer = m_pStreamViewer;
    m_pStreamViewer = pStreamViewer;

    // Synchronize the skeleton pause status with the stream pause status
    if (m_pStreamViewer)
    {
        m_pStreamViewer->PauseSkeleton(m_paused);
    }

    return pOldViewer;
}*/

/* ---------------------------------------------------------------- */

/*HANDLE KinectStream::next_kinect_event = INVALID_HANDLE_VALUE;

KinectStream::KinectStream() :
    cout_update(0),
    nui_sensor(nullptr),
    D2DFactory(nullptr)
{
}

KinectStream::~KinectStream()
{
    kinect_stop();
    //nui_sensor->NuiShutdown(); // possible deadlock -> Dev Microsoft = Je code avec le cul
    //safe_release(nui_sensor);
    //DiscardDirect2DResources();
    //safe_release(D2DFactory);
}

void KinectStream::start()
{
    m_hStopStreamEventThread = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    CreateThread(nullptr, 0, KinectStream::kinect_main, reinterpret_cast<void*>(this), 0, &tId);
}

DWORD WINAPI KinectStream::kinect_main(LPVOID lpParam)
{
    KinectStream* pThis = reinterpret_cast<KinectStream*>(lpParam);

    pThis->nui_sensor = nullptr;
    
    pThis->kinect_init(pThis);
    pThis->kinect_start(pThis);

    HANDLE events[] = { pThis->m_hStopStreamEventThread,
                        pThis->next_kinect_event };

    while(true)
    {
        DWORD ret = WaitForMultipleObjects(ARRAYSIZE(events), events, FALSE, INFINITE);

        if (WAIT_OBJECT_0 == ret)
            continue;

        if (WAIT_OBJECT_0 + 1 == ret)
        {
            std::cout << "TIMER" << std::endl;
            //SendMessage(pThis->GetWindow(), WM_TIMEREVENT, 0, 0);
        }
        else if (WAIT_OBJECT_0 + 4 >= ret)
        {
            std::cout << "STREAM" << std::endl;
            pThis->kinect_update(pThis);
            //SendMessage(pThis->GetWindow(), WM_STREAMEVENT, 0, 0);
        }
    }

    //pThis->KinectStream_stop();

    return 0;
}

HRESULT KinectStream::kinect_init(KinectStream* pThis)
{
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &D2DFactory);

    INuiSensor* pNuiSensor;

    int sensor_count = 0;
    HRESULT hr = NuiGetSensorCount(&sensor_count);

    if (FAILED(hr))
        return hr;

    // Verification de tout les capteurs de la KinectStream
    for (int i = 0; i < sensor_count; ++i)
    {
        // Création des capteur pour vérifier leur état
        hr = NuiCreateSensorByIndex(i, &pNuiSensor);
        if (FAILED(hr))
            continue;

        // Récupération du status du capteur, si connecté -> initalisation
        hr = pNuiSensor->NuiStatus();

        if (hr == S_OK)
        {
            std::cout << "device connection ID: " << pNuiSensor->NuiDeviceConnectionId() << std::endl;
            pThis->nui_sensor = pNuiSensor;
            break;
        }

        // Capteur non fonctionnel, détachement
        pNuiSensor->Release();
    }

    if (pThis->nui_sensor != nullptr)
    {
        // Initialisation du capteur du squelette
        hr = pThis->nui_sensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_SKELETON);

        if (SUCCEEDED(hr))
        {
            //set_status_message("KinectStream initialisée");
            std::cout << "KinectStream initialisee" << std::endl;
            return hr;
        }
    }

    if (pThis->nui_sensor == nullptr || FAILED(hr))
    {
        //set_status_message("Aucune KinectStream trouvée");
        std::cout << "Aucune KinectStream trouvee" << std::endl;
        return E_FAIL;
    }
}

void KinectStream::kinect_start(KinectStream* pThis)
{
    if(pThis->nui_sensor != nullptr)
    {
        // Création d'un évenement pour signaler de nouvelles données du squette
        pThis->next_kinect_event = CreateEvent(nullptr, TRUE, FALSE, nullptr);

        // Activation du flux de données du squelette
        HRESULT hr = pThis->nui_sensor->NuiSkeletonTrackingEnable(pThis->next_kinect_event, 0);

        //set_status_message("KinectStream démarrée");
        std::cout << "KinectStream démarrée" << std::endl;
    }
}

void KinectStream::kinect_stop()
{
    if(nui_sensor != nullptr)
    {
        ResetEvent(next_kinect_event);

        nui_sensor->NuiSkeletonTrackingDisable();
        cout_update = 0;
        std::cout << "\rx0                        ";

        //set_status_message("KinectStream arrêtée");
        std::cout << "KinectStream arrêtée" << std::endl;
    }
}

void KinectStream::kinect_update(KinectStream* pThis)
{
    if(pThis->nui_sensor == nullptr)
        return;

    // Attente de traitement des données du squelette
    //if(WaitForSingleObject(next_KinectStream_event, INFINITE) == WAIT_OBJECT_0)
    //{
        cout_update++;
        std::cout << "\rx" << cout_update;
        //ProcessSkeleton();
    //}

    if (WAIT_OBJECT_0 == WaitForSingleObject(pThis->next_kinect_event, 0))
    {
        // if we have received any valid new depth data we may need to draw
        //ProcessSkeleton();
        std::cout << "update" << std::endl;
    }
}*/