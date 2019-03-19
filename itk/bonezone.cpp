// Biodinamics Lab DSX Optimization Program
// Linghai Wang and George Stetten
// Copyright 2018-2019

#include <stdio.h>
#include <iostream>

#include "dsxGUI.h"
#include "dsxProject.h"

vtkStandardNewMacro(dsxGrabBufferActor);
vtkStandardNewMacro(dsxGrabBufferCamera);
vtkStandardNewMacro(dsxTiffMovie);

int main(int argc, char *argv[])
{
    if(argc > 1)
    {
        std::cerr << "Usage: " << argv[0] << " (no longer takes filenames as command arguments)" << std::endl;
        return(EXIT_FAILURE);
    }

    std::cerr << "bonezone: loading data, please wait..." << std::endl;

    dsxProject   * project   = new dsxProject();
    dsxGUI       * gui       = new dsxGUI();
    
    project->Initialize(gui, argc, argv);  //Load in data
    gui->Initialize(project);              //George's new single-window system
    project->RunMultiWindowSystem();       //Linghai's original system
/*
 //MOVE THIS TO dsxProject
    while(optimizer->MetricAboveThreshold())
    {
        if (gui->TrackballMoved())
        {
            optimizer->UpdatePoseFromTrackball();
        }
        optimizer->IterateOpitmizer();
    }
*/
    return(EXIT_SUCCESS);
}
