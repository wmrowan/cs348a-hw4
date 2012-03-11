
#ifdef sgi
#include "forms.h"

typedef FL_FORM menuType;

extern menuType * create_form_menuForm ();

extern FL_OBJECT
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
#else
typedef char menuType;
#endif

/*---------------------------------------------------------------------------*/

extern int        noAlgorithmLoop ();
