#include "dsxVolumeSpinner.h"

dsxVolumeSpinner::dsxVolumeSpinner()
{
}

dsxVolumeSpinner::~dsxVolumeSpinner()
{
}

void dsxVolumeSpinner::Execute(vtkObject * caller, unsigned long eventId, void * vtkNotUsed(callData))
{
    if(eventId == vtkCommand::TimerEvent)
    {
        if(eventId != vtkCommand::LeftButtonPressEvent)
        {
            ctVolume->RotateX(2);//this rotates around the world axis
        }
        ctInlineWindow->Render();
        ctOffsetWindow->Render();
    }
}


