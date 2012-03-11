/* Form definition file generated with fdesign. */

#include "forms.h"

FL_OBJECT
        *minmaxerForm,
        *minmaxerInputGroup,
        *b112,
        *b401,
        *minmaxerComputationGroup,
        *b302,
        *b303,
        *b400,
        *b306,
        *bAlgorithm,
        *minmaxerInfoGroup,
        *bFname,
        *bNofVertices,
        *bNofEdges,
        *bNofCHedges,
        *minmaxerPropertiesGroup,
        *b304,
        *b307,
        *b308,
        *b311,
        *b310,
        *b309,
        *minmaxerAlgorithmsGroup,
        *b107,
        *b106,
        *b105,
        *b104,
        *b103,
        *b109,
        *b108,
        *b102,
        *b101,
        *b110,
        *minmaxerBigButtons,
        *b111,
        *b100,
        *b209,
        *minmaxerRenderGroup,
        *b200,
        *b201,
        *b204,
        *b203,
        *b205,
        *b206,
        *b208,
        *b207,
        *b202;

FL_FORM *
create_form_menuForm()
{
  FL_OBJECT *obj;
  FL_FORM *menuForm;

  menuForm = fl_bgn_form(FL_NO_BOX,270.0,980.0);
  minmaxerForm = obj = fl_add_box(FL_UP_BOX,0.0,0.0,270.0,980.0,"");
    fl_set_object_color(obj,12,12);
    fl_set_object_lcol(obj,4);
    fl_set_object_lstyle(obj,FL_ENGRAVED_STYLE);
  minmaxerInputGroup = fl_bgn_group();
  obj = fl_add_box(FL_FRAME_BOX,10.0,250.0,250.0,180.0,"");
    fl_set_object_color(obj,10,47);
  b112 = obj = fl_add_input(FL_NORMAL_INPUT,20.0,320.0,230.0,40.0,"");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_color(obj,10,188);
    fl_set_object_align(obj,FL_ALIGN_CENTER);
  obj = fl_add_text(FL_NORMAL_TEXT,40.0,380.0,180.0,20.0,"Save Triangulation in File:");
    fl_set_object_lcol(obj,4);
    fl_set_object_lstyle(obj,FL_ENGRAVED_STYLE);
  b401 = obj = fl_add_button(FL_NORMAL_BUTTON,60.0,270.0,150.0,30.0,"Cancel");
    fl_set_object_lcol(obj,4);
    fl_set_object_lstyle(obj,FL_ENGRAVED_STYLE);
  fl_end_group();
  obj = fl_add_box(FL_FRAME_BOX,10.0,510.0,250.0,290.0,"");
    fl_set_object_color(obj,12,47);
  minmaxerComputationGroup = fl_bgn_group();
  b302 = obj = fl_add_slider(FL_HOR_NICE_SLIDER,20.0,670.0,230.0,30.0,"Computation Speed:                        ");
    fl_set_object_color(obj,188,4);
    fl_set_object_lcol(obj,4);
    fl_set_object_align(obj,FL_ALIGN_TOP);
    fl_set_object_lstyle(obj,FL_ENGRAVED_STYLE);
  obj = fl_add_text(FL_NORMAL_TEXT,20.0,770.0,20.0,20.0,"Current Algorithm:");
    fl_set_object_lcol(obj,4);
    fl_set_object_lstyle(obj,FL_ENGRAVED_STYLE);
  b303 = obj = fl_add_button(FL_PUSH_BUTTON,20.0,600.0,100.0,60.0,"Pause");
    fl_set_object_lcol(obj,4);
    fl_set_object_lstyle(obj,FL_ENGRAVED_STYLE);
  b400 = obj = fl_add_button(FL_NORMAL_BUTTON,150.0,600.0,100.0,60.0,"Interrupt");
    fl_set_object_lcol(obj,4);
    fl_set_object_lstyle(obj,FL_ENGRAVED_STYLE);
  b306 = obj = fl_add_button(FL_NORMAL_BUTTON,20.0,520.0,230.0,70.0,"Step");
    fl_set_object_lcol(obj,4);
    fl_set_object_lstyle(obj,FL_ENGRAVED_STYLE);
  bAlgorithm = obj = fl_add_text(FL_NORMAL_TEXT,20.0,730.0,230.0,30.0,"Text");
    fl_set_object_boxtype(obj,FL_DOWN_BOX);
    fl_set_object_color(obj,188,47);
  fl_end_group();
  obj = fl_add_box(FL_FLAT_BOX,10.0,940.0,250.0,30.0,"MINMAXer  v 1.02");
    fl_set_object_color(obj,12,47);
    fl_set_object_lcol(obj,4);
    fl_set_object_lstyle(obj,FL_ENGRAVED_STYLE);
  minmaxerInfoGroup = fl_bgn_group();
  obj = fl_add_box(FL_FRAME_BOX,10.0,810.0,250.0,130.0,"");
    fl_set_object_color(obj,12,47);
  obj = fl_add_text(FL_NORMAL_TEXT,10.0,910.0,51.9,20.0,"File: ");
    fl_set_object_lcol(obj,4);
    fl_set_object_lstyle(obj,FL_ENGRAVED_STYLE);
  bFname = obj = fl_add_text(FL_NORMAL_TEXT,50.0,910.0,200.0,20.0,"Text");
    fl_set_object_boxtype(obj,FL_DOWN_BOX);
    fl_set_object_color(obj,188,47);
  obj = fl_add_text(FL_NORMAL_TEXT,10.0,880.0,20.8,20.0,"Vertices: ");
    fl_set_object_lcol(obj,4);
    fl_set_object_lstyle(obj,FL_ENGRAVED_STYLE);
  bNofVertices = obj = fl_add_text(FL_NORMAL_TEXT,100.0,880.0,150.0,20.0,"Text");
    fl_set_object_boxtype(obj,FL_DOWN_BOX);
    fl_set_object_color(obj,188,47);
  obj = fl_add_text(FL_NORMAL_TEXT,10.0,850.0,10.4,20.0,"Edges: ");
    fl_set_object_lcol(obj,4);
    fl_set_object_lstyle(obj,FL_ENGRAVED_STYLE);
  bNofEdges = obj = fl_add_text(FL_NORMAL_TEXT,100.0,850.0,150.0,20.0,"Text");
    fl_set_object_boxtype(obj,FL_DOWN_BOX);
    fl_set_object_color(obj,188,47);
  obj = fl_add_text(FL_NORMAL_TEXT,10.0,820.0,10.4,20.0,"CH-Edges: ");
    fl_set_object_lcol(obj,4);
    fl_set_object_lstyle(obj,FL_ENGRAVED_STYLE);
  bNofCHedges = obj = fl_add_text(FL_NORMAL_TEXT,100.0,820.0,150.0,20.0,"Text");
    fl_set_object_boxtype(obj,FL_DOWN_BOX);
    fl_set_object_color(obj,188,47);
  fl_end_group();
  minmaxerPropertiesGroup = fl_bgn_group();
  obj = fl_add_box(FL_FRAME_BOX,10.0,250.0,250.0,180.0,"");
    fl_set_object_color(obj,12,47);
  obj = fl_add_text(FL_NORMAL_TEXT,70.0,399.8,180.0,17.1,"Nice <-> Sloppy");
    fl_set_object_lcol(obj,4);
    fl_set_object_lstyle(obj,FL_ENGRAVED_STYLE);
  obj = fl_add_text(FL_NORMAL_TEXT,70.0,373.0,180.0,17.1,"Spin");
    fl_set_object_lcol(obj,4);
    fl_set_object_lstyle(obj,FL_ENGRAVED_STYLE);
  obj = fl_add_text(FL_NORMAL_TEXT,70.0,346.2,180.0,17.1,"Auto Wireframe");
    fl_set_object_lcol(obj,4);
    fl_set_object_lstyle(obj,FL_ENGRAVED_STYLE);
  obj = fl_add_text(FL_NORMAL_TEXT,70.0,319.5,180.0,17.1,"Show Vertices");
    fl_set_object_lcol(obj,4);
    fl_set_object_lstyle(obj,FL_ENGRAVED_STYLE);
  obj = fl_add_text(FL_NORMAL_TEXT,70.0,292.7,180.0,17.1,"Show Lifted Vertices");
    fl_set_object_lcol(obj,4);
    fl_set_object_lstyle(obj,FL_ENGRAVED_STYLE);
  obj = fl_add_text(FL_NORMAL_TEXT,70.0,265.9,180.0,17.1,"Weights (Z-coord) Off");
    fl_set_object_lcol(obj,4);
    fl_set_object_lstyle(obj,FL_ENGRAVED_STYLE);
  b304 = obj = fl_add_lightbutton(FL_PUSH_BUTTON,20.0,399.8,50.0,17.1,"");
    fl_set_object_color(obj,59,3);
    fl_set_object_lcol(obj,6);
  b307 = obj = fl_add_lightbutton(FL_PUSH_BUTTON,20.0,373.0,50.0,17.1,"");
    fl_set_object_color(obj,59,3);
    fl_set_object_lcol(obj,6);
  b308 = obj = fl_add_lightbutton(FL_PUSH_BUTTON,20.0,346.2,50.0,17.1,"");
    fl_set_object_color(obj,59,3);
    fl_set_object_lcol(obj,6);
  b311 = obj = fl_add_lightbutton(FL_PUSH_BUTTON,20.0,319.5,50.0,17.1,"");
    fl_set_object_color(obj,59,3);
    fl_set_object_lcol(obj,6);
  b310 = obj = fl_add_lightbutton(FL_PUSH_BUTTON,20.0,265.9,50.0,17.1,"");
    fl_set_object_color(obj,59,3);
    fl_set_object_lcol(obj,6);
  b309 = obj = fl_add_lightbutton(FL_PUSH_BUTTON,20.0,292.7,50.0,17.1,"");
    fl_set_object_color(obj,59,3);
    fl_set_object_lcol(obj,6);
  fl_end_group();
  minmaxerAlgorithmsGroup = fl_bgn_group();
  obj = fl_add_text(FL_NORMAL_TEXT,65.0,750.0,166.2,16.2,"Remove Duplicate Vertices");
    fl_set_object_lcol(obj,4);
    fl_set_object_lstyle(obj,FL_ENGRAVED_STYLE);
  obj = fl_add_text(FL_NORMAL_TEXT,65.0,725.0,166.2,16.2,"Initial (Plane Sweep)");
    fl_set_object_lcol(obj,4);
    fl_set_object_lstyle(obj,FL_ENGRAVED_STYLE);
  obj = fl_add_text(FL_NORMAL_TEXT,65.0,700.0,166.2,16.2,"Delaunay (Lawson Flip)");
    fl_set_object_lcol(obj,4);
    fl_set_object_lstyle(obj,FL_ENGRAVED_STYLE);
  obj = fl_add_text(FL_NORMAL_TEXT,65.0,675.0,166.2,16.2,"Delaunay (Incremental Flip)");
    fl_set_object_lcol(obj,4);
    fl_set_object_lstyle(obj,FL_ENGRAVED_STYLE);
  obj = fl_add_text(FL_NORMAL_TEXT,65.4,651.5,166.2,16.2,"Regular (Incremental Flip)");
    fl_set_object_lcol(obj,4);
    fl_set_object_lstyle(obj,FL_ENGRAVED_STYLE);
  obj = fl_add_text(FL_NORMAL_TEXT,65.4,626.2,166.2,16.2,"MinMax Angle O(n^2 log n)");
    fl_set_object_lcol(obj,4);
    fl_set_object_lstyle(obj,FL_ENGRAVED_STYLE);
  obj = fl_add_text(FL_NORMAL_TEXT,65.4,600.9,166.2,16.2,"MinMax Angle O(n^3)");
    fl_set_object_lcol(obj,4);
    fl_set_object_lstyle(obj,FL_ENGRAVED_STYLE);
  obj = fl_add_text(FL_NORMAL_TEXT,65.4,575.6,166.2,16.2,"MaxMin Height O(n^2 log n)");
    fl_set_object_lcol(obj,4);
    fl_set_object_lstyle(obj,FL_ENGRAVED_STYLE);
  obj = fl_add_text(FL_NORMAL_TEXT,65.4,550.3,166.2,16.2,"MaxMin Height O(n^3)");
    fl_set_object_lcol(obj,4);
    fl_set_object_lstyle(obj,FL_ENGRAVED_STYLE);
  obj = fl_add_text(FL_NORMAL_TEXT,67.6,525.5,166.2,16.2,"MinMax Slope O(n^3)");
    fl_set_object_lcol(obj,4);
    fl_set_object_lstyle(obj,FL_ENGRAVED_STYLE);
  obj = fl_add_text(FL_NORMAL_TEXT,93.1,773.8,73.8,16.2,"Algorithms");
    fl_set_object_lcol(obj,4);
    fl_set_object_lstyle(obj,FL_ENGRAVED_STYLE);
  b107 = obj = fl_add_button(FL_NORMAL_BUTTON,20.0,525.0,48.0,15.5,"");
  b106 = obj = fl_add_button(FL_NORMAL_BUTTON,20.0,550.0,48.0,15.5,"");
  b105 = obj = fl_add_button(FL_NORMAL_BUTTON,20.0,575.0,48.0,15.5,"");
  b104 = obj = fl_add_button(FL_NORMAL_BUTTON,20.0,600.0,48.0,15.5,"");
  b103 = obj = fl_add_button(FL_NORMAL_BUTTON,20.0,625.0,48.0,15.5,"");
  b109 = obj = fl_add_button(FL_NORMAL_BUTTON,20.0,650.0,48.0,15.5,"");
  b108 = obj = fl_add_button(FL_NORMAL_BUTTON,20.0,675.0,48.0,15.5,"");
  b102 = obj = fl_add_button(FL_NORMAL_BUTTON,20.0,700.0,48.0,15.5,"");
  b101 = obj = fl_add_button(FL_NORMAL_BUTTON,20.0,725.0,48.0,15.5,"");
  b110 = obj = fl_add_button(FL_NORMAL_BUTTON,20.0,750.0,48.0,15.5,"");
  fl_end_group();
  minmaxerBigButtons = fl_bgn_group();
  b111 = obj = fl_add_button(FL_NORMAL_BUTTON,20.0,474.3,140.0,25.7,"Save Triangulation");
    fl_set_object_lcol(obj,4);
    fl_set_object_lstyle(obj,FL_ENGRAVED_STYLE);
  b100 = obj = fl_add_button(FL_NORMAL_BUTTON,180.0,474.3,70.0,25.7,"Quit");
    fl_set_object_lcol(obj,4);
    fl_set_object_lstyle(obj,FL_ENGRAVED_STYLE);
  b209 = obj = fl_add_button(FL_PUSH_BUTTON,20.0,440.0,230.0,25.7,
			     "Update Edge Flips");
    fl_set_object_lcol(obj,4);
    fl_set_object_lstyle(obj,FL_ENGRAVED_STYLE);
  fl_end_group();
  minmaxerRenderGroup = fl_bgn_group();
  obj = fl_add_box(FL_FRAME_BOX,10.0,10.0,250.0,230.0,"");
    fl_set_object_color(obj,12,47);
  obj = fl_add_text(FL_NORMAL_TEXT,70.0,217.3,180.0,16.2,"Vertices Only");
    fl_set_object_lcol(obj,4);
    fl_set_object_lstyle(obj,FL_ENGRAVED_STYLE);
  obj = fl_add_text(FL_NORMAL_TEXT,70.0,192.0,180.0,16.2,"Wireframe");
    fl_set_object_lcol(obj,4);
    fl_set_object_lstyle(obj,FL_ENGRAVED_STYLE);
  obj = fl_add_text(FL_NORMAL_TEXT,70.0,166.7,180.0,16.2,"Lighting");
    fl_set_object_lcol(obj,4);
    fl_set_object_lstyle(obj,FL_ENGRAVED_STYLE);
  obj = fl_add_text(FL_NORMAL_TEXT,70.0,141.5,180.0,16.2,"Hidden Line (version 1)");
    fl_set_object_lcol(obj,4);
    fl_set_object_lstyle(obj,FL_ENGRAVED_STYLE);
  obj = fl_add_text(FL_NORMAL_TEXT,70.0,116.2,180.0,16.2,"Hidden Line (version 2)");
    fl_set_object_lcol(obj,4);
    fl_set_object_lstyle(obj,FL_ENGRAVED_STYLE);
  obj = fl_add_text(FL_NORMAL_TEXT,70.0,90.9,180.0,16.2,"Minimum Angle");
    fl_set_object_lcol(obj,4);
    fl_set_object_lstyle(obj,FL_ENGRAVED_STYLE);
  obj = fl_add_text(FL_NORMAL_TEXT,70.0,65.6,180.0,16.2,"Maximum Angle");
    fl_set_object_lcol(obj,4);
    fl_set_object_lstyle(obj,FL_ENGRAVED_STYLE);
  obj = fl_add_text(FL_NORMAL_TEXT,70.0,40.3,180.0,16.2,"Minimum Height");
    fl_set_object_lcol(obj,4);
    fl_set_object_lstyle(obj,FL_ENGRAVED_STYLE);
  obj = fl_add_text(FL_NORMAL_TEXT,70.0,15.1,180.0,16.2,"Maximum Slope");
    fl_set_object_lcol(obj,4);
    fl_set_object_lstyle(obj,FL_ENGRAVED_STYLE);
  b200 = obj = fl_add_lightbutton(FL_RADIO_BUTTON,20.0,217.3,50.0,16.2,"");
    fl_set_object_color(obj,1,3);
    fl_set_object_lcol(obj,6);
  b201 = obj = fl_add_lightbutton(FL_RADIO_BUTTON,20.0,192.0,50.0,16.2,"");
    fl_set_object_color(obj,1,3);
    fl_set_object_lcol(obj,6);
  b204 = obj = fl_add_lightbutton(FL_RADIO_BUTTON,20.0,166.7,50.0,16.2,"");
    fl_set_object_color(obj,1,3);
    fl_set_object_lcol(obj,6);
  b203 = obj = fl_add_lightbutton(FL_RADIO_BUTTON,20.0,141.5,50.0,16.2,"");
    fl_set_object_color(obj,1,3);
    fl_set_object_lcol(obj,6);
  b205 = obj = fl_add_lightbutton(FL_RADIO_BUTTON,20.0,90.9,50.0,16.2,"");
    fl_set_object_color(obj,1,3);
    fl_set_object_lcol(obj,6);
  b206 = obj = fl_add_lightbutton(FL_RADIO_BUTTON,20.0,65.6,50.0,16.2,"");
    fl_set_object_color(obj,1,3);
    fl_set_object_lcol(obj,6);
  b208 = obj = fl_add_lightbutton(FL_RADIO_BUTTON,20.0,15.1,50.0,16.2,"");
    fl_set_object_color(obj,1,3);
    fl_set_object_lcol(obj,6);
  b207 = obj = fl_add_lightbutton(FL_RADIO_BUTTON,20.0,40.3,50.0,16.2,"");
    fl_set_object_color(obj,1,3);
    fl_set_object_lcol(obj,6);
  b202 = obj = fl_add_lightbutton(FL_RADIO_BUTTON,20.0,116.0,50.0,16.2,"");
    fl_set_object_color(obj,1,3);
    fl_set_object_lcol(obj,6);
  fl_end_group();
  fl_end_form();

  return menuForm;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
