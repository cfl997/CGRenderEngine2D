#include "CGRenderEventDefault.h"

#include "MouseEvent.h"
#include "KeyEvent.h"
#include "WindowsEvent.h"

CGRender::CGRenderEventDefault::CGRenderEventDefault(Window* window) :super(CGRenderEventType::RenderEvent_Default, window)
{

}

CGRender::CGRenderEventDefault::~CGRenderEventDefault()
{
}
