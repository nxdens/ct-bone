#include "dsxReRenderer.h"

dsxReRenderer::dsxReRenderer()
{
}

dsxReRenderer::~dsxReRenderer()
{
}

void dsxReRenderer::Execute(vtkObject *caller, unsigned long eventId, void * vtkNotUsed(callData))
{
    //cout << "modded" << endl;
    //this is jagged when attached to the volume modified and i suspect that it is due to the fact that it gets called too often
    otherRenderer->Render();//this is a really slow action for some reason
}
