#include "tck/kinect/kinect.hpp"
#include <iostream>

HANDLE kinect::next_kinect_event = INVALID_HANDLE_VALUE;

kinect::kinect() :
    cout_update(0),
    nui_sensor(NULL),
    D2DFactory(NULL)
{
}

kinect::~kinect()
{
    kinect_stop();
    //nui_sensor->NuiShutdown(); // possible deadlock -> Dev Microsoft = Je code avec le cul
    /*safe_release(nui_sensor);
    DiscardDirect2DResources();
    safe_release(D2DFactory);*/
}

void kinect::start()
{
    CreateThread(NULL, 0, kinect::kinect_main, reinterpret_cast<void*>(this), 0, &tId);
}

DWORD WINAPI kinect::kinect_main(LPVOID lpParam)
{
    kinect* pThis = reinterpret_cast<kinect*>(lpParam);
    
    pThis->kinect_init();
    pThis->kinect_start();

    const int event_count = 1;
    HANDLE events[event_count];

    while(true)
    {
        events[0] = pThis->next_kinect_event;
        //MsgWaitForMultipleObjects(event_count, events, FALSE, INFINITE, QS_ALLINPUT);
        DWORD ret = WaitForMultipleObjects(ARRAYSIZE(events), events, FALSE, INFINITE);

        /*if (WAIT_OBJECT_0 == ret)
            break;*/
        if (WAIT_OBJECT_0 + 1 == ret)
        {
            std::cout << "TIMER" << std::endl;
            //SendMessageW(pThis->GetWindow(), WM_TIMEREVENT, 0, 0);
        }
        else if (WAIT_OBJECT_0 + 4 >= ret)
        {
            std::cout << "STREAM" << std::endl;
            pThis->kinect_update();
            //SendMessageW(pThis->GetWindow(), WM_STREAMEVENT, 0, 0);
        }
    }

    pThis->kinect_stop();

    return 0;
}

/**
* Fonction d'initalisation de la Kinect.
**/
HRESULT kinect::kinect_init()
{
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &D2DFactory);

    INuiSensor* pNuiSensor;

    int sensor_count = 0;
    HRESULT hr = NuiGetSensorCount(&sensor_count);

    if (FAILED(hr))
        return hr;

    // Verification de tout les capteurs de la Kinect
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
            nui_sensor = pNuiSensor;
            break;
        }

        // Capteur non fonctionnel, détachement
        pNuiSensor->Release();
    }

    if (nui_sensor != NULL)
    {
        // Initialisation du capteur du squelette
        hr = nui_sensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_SKELETON);

        if (SUCCEEDED(hr))
        {
            //set_status_message("Kinect initialisée");
            std::cout << "Kinect initialisée" << std::endl;
            return hr;
        }
    }

    if (nui_sensor == NULL || FAILED(hr))
    {
        //set_status_message("Aucune Kinect trouvée");
        std::cout << "Aucune Kinect trouvée" << std::endl;
        return E_FAIL;
    }
}

/**
* Fonction de démarrage de la Kinect.
**/
void kinect::kinect_start()
{
    if (nui_sensor != NULL)
    {
        // Création d'un évenement pour signaler de nouvelles données du squette
        next_kinect_event = CreateEvent(NULL, TRUE, FALSE, NULL);

        // Activation du flux de données du squelette
        /*hr =*/ nui_sensor->NuiSkeletonTrackingEnable(next_kinect_event, 0);

        //set_status_message("Kinect démarrée");
        std::cout << "Kinect démarrée" << std::endl;
    }
}

/**
* Fonction d'arret de la Kinect.
**/
void kinect::kinect_stop()
{
    if(nui_sensor != NULL)
    {
        ResetEvent(next_kinect_event);

        nui_sensor->NuiSkeletonTrackingDisable();
        cout_update = 0;
        std::cout << "\rx0                        ";

        //set_status_message("Kinect arrêtée");
        std::cout << "Kinect arrêtée" << std::endl;
    }
}

/**
* Fonction de traitement des données Kinect.
**/
void kinect::kinect_update()
{
    if(nui_sensor == NULL)
        return;

    next_kinect_event;

    // Attente de traitement des données du squelette
    if(WaitForSingleObject(next_kinect_event, INFINITE) == WAIT_OBJECT_0)
    {
        //cout_update++;
        //std::cout << "\rx" << cout_update;
        //ProcessSkeleton();
    }
}