#ifndef __CEGUI_widgets_h_
#define __CEGUI_widgets_h_

#include <CEGUIWindow.h>

class AxisIndicator : public CEGUI::Window
{
public:
    //! 'Namespace' string used for global events on this class.
	  static const CEGUI::String EventNamespace;
    //! String holding the type name of this widget.
    static const CEGUI::String WidgetTypeName;
     //! Widget name suffix for the crosshair component.
    static const CEGUI::String CrosshairNameSuffix;

    CEGUI::Window* getCrosshair() const;
    CEGUI::UVector2 getAxisPosition() { return d_axis_point; }
    bool setAxisPosition(CEGUI::UVector2& position);

    AxisIndicator(const CEGUI::String& type, const CEGUI::String& name);
    ~AxisIndicator();
 
protected:
    // overridden from base class.
    void drawSelf(const CEGUI::RenderingContext& ctx);
    bool testClassName_impl(const CEGUI::String& class_name) const;

    CEGUI::UVector2 d_axis_point;
};

class HatIndicator : public CEGUI::Window
{
public:
    //! 'Namespace' string used for global events on this class.
	  static const CEGUI::String EventNamespace;
    //! String holding the type name of this widget.
    static const CEGUI::String WidgetTypeName;

    static const CEGUI::String topNameSuffix;
    static const CEGUI::String toprightNameSuffix;
    static const CEGUI::String rightNameSuffix;
    static const CEGUI::String bottomrightNameSuffix;
    static const CEGUI::String bottomNameSuffix;
    static const CEGUI::String bottomleftNameSuffix;
    static const CEGUI::String leftNameSuffix;
    static const CEGUI::String topleftNameSuffix;
    
    CEGUI::Window* getHatWindow(int pos) const;
    int getHatPosition() { return d_hat_point; }
    bool setHatPosition(int position);

    HatIndicator(const CEGUI::String& type, const CEGUI::String& name);
    ~HatIndicator();
 
protected:
    // overridden from base class.
    void drawSelf(const CEGUI::RenderingContext& ctx);
    bool testClassName_impl(const CEGUI::String& class_name) const;

    int d_hat_point;
};

#endif // #ifndef __CEGUI_widgets_h_