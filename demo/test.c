/*****************************************************************************
*   small test for expr2tree.						     *
*                                                                            *
* Written by:  Gershon Elber                           Ver 0.1, Mar  1988    *
*****************************************************************************/

#include <stdio.h>
#include "expr2tree.h"

main()
{
    e2t_expr_node *tree = NULL, *drv_tree = NULL;
    double x;
    char s[128];

    e2t_setparamvalue(0, E2T_PARAM_X);
    e2t_setparamvalue(0, E2T_PARAM_Y);
    e2t_setparamvalue(0, E2T_PARAM_Z);

    while (1) {
        printf("Enter func:");
	gets(s);
        tree = e2t_expr2tree(s);
        if (!tree) {
             printf("Error %d\n", e2t_parsing_error);
             continue;
	}
        printf("The tree is:");
        e2t_printtree(tree, (char *) NULL);

        double drv_val = 0.;
        printf("\nEnter x value:"); gets(s);
        sscanf(s, "%lf", &x);
        printf("\n\nDtree/Dx at %f is:", x);
        drv_tree = e2t_derivtree(tree, E2T_PARAM_X);
        e2t_setparamvalue(x, E2T_PARAM_X);
        e2t_printtree(drv_tree, (char *) NULL);
        printf("   == %lf\n\n", e2t_evaltree(drv_tree));

        e2t_setparamvalue(x, E2T_PARAM_X);
        printf("Tree value for x = %lf is %lf\n", x, e2t_evaltree(tree));
    }
}
