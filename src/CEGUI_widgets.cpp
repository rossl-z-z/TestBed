#include "Log.h"
#include <CEGUIRenderingWindow.h>
#include <CEGUIRenderingContext.h>
#include <CEGUIExceptions.h>
#define M_PI       3.14159265358979323846
#include "falagard/CEGUIFalWidgetLookManager.h"
#include "falagard/CEGUIFalWidgetLookFeel.h"
#include "CEGUIWindowManager.h"
#include "CEGUI_widgets.h"
 
using namespace CEGUI;
 
const String AxisIndicator::EventNamespace("AxisIndicator");
const String AxisIndicator::WidgetTypeName("AxisIndicator");
const String AxisIndicator::CrosshairNameSuffix("__auto_crosshair__");


AxisIndicator::AxisIndicator(const CEGUI::String& type, const CEGUI::String& name) :
    Window(type, name),
    d_axis_point(cegui_reldim(0.5), cegui_reldim(0.5))
{
}

AxisIndicator::~AxisIndicator()
{
}

CEGUI::Window* AxisIndicator::getCrosshair() const
{
  return static_cast<CEGUI::Window*>(WindowManager::getSingleton().getWindow(getName() + CrosshairNameSuffix));
}

bool AxisIndicator::setAxisPosition(CEGUI::UVector2& position)
{
  const CEGUI::UVector2 old_pos = d_axis_point;

  // limit position to valid range ?
  d_axis_point = position;

  return d_axis_point != old_pos;
}

void AxisIndicator::drawSelf(const RenderingContext& ctx)
{
  CEGUI::Window* cross = getCrosshair();
  cross->setPosition(d_axis_point);
  Window::drawSelf(ctx);
}
 
bool AxisIndicator::testClassName_impl(const CEGUI::String& class_name) const
{
    if (class_name == AxisIndicator::WidgetTypeName)
        return true;
 
    return Window::testClassName_impl(class_name);
}
 

const String HatIndicator::EventNamespace("HatIndicator");
const String HatIndicator::WidgetTypeName("HatIndicator");
const String HatIndicator::topNameSuffix("__auto_top__");
const String HatIndicator::toprightNameSuffix("__auto_topright__");
const String HatIndicator::rightNameSuffix("__auto_right__");
const String HatIndicator::bottomrightNameSuffix("__auto_bottomright__");
const String HatIndicator::bottomNameSuffix("__auto_bottom__");
const String HatIndicator::bottomleftNameSuffix("__auto_bottomleft__");
const String HatIndicator::leftNameSuffix("__auto_left__");
const String HatIndicator::topleftNameSuffix("__auto_topleft__");

HatIndicator::HatIndicator(const CEGUI::String& type, const CEGUI::String& name) :
    Window(type, name),
    d_hat_point(0)
{
}

HatIndicator::~HatIndicator()
{
}

CEGUI::Window* HatIndicator::getHatWindow(int pos) const
{
  String positions[] = { topNameSuffix, toprightNameSuffix, rightNameSuffix, bottomrightNameSuffix,
    bottomNameSuffix, bottomleftNameSuffix, leftNameSuffix, topleftNameSuffix };
  if (pos >= 0 && pos < 8)
    return static_cast<CEGUI::Window*>(WindowManager::getSingleton().getWindow(getName() + positions[pos]));
  else
    return NULL;
}

bool HatIndicator::setHatPosition(int position)
{
  const int old_pos = d_hat_point;

  // limit position to valid range ?
  d_hat_point = position;

  return d_hat_point != old_pos;
}

void HatIndicator::drawSelf(const RenderingContext& ctx)
{
  CEGUI::UVector2 pos[8];
	for(int i = 0; i < 8; i++)
	{
		int xPos = (int)(sin(-2 * M_PI * i / 8 + M_PI) * 20.0) + 20;
		int yPos = (int)(cos(2 * M_PI * i / 8 + M_PI) * 20.0) + 20;

    pos[i].d_x = cegui_absdim(xPos);
    pos[i].d_y = cegui_absdim(yPos);
	}
  CEGUI::Window* w;

  for (int i = 0; i < 8; i++) {
    w = getHatWindow(i);
    w->setPosition(pos[i]);
    if (i == d_hat_point)
      w->disable();
    else
      w->enable();
  }
  Window::drawSelf(ctx);
}
 
bool HatIndicator::testClassName_impl(const CEGUI::String& class_name) const
{
    if (class_name == HatIndicator::WidgetTypeName)
        return true;
 
    return Window::testClassName_impl(class_name);
}
 