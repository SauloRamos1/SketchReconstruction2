#include "reconstruction.h"

Reconstruction::Reconstruction()
{

}


void Reconstruction::estimateShape(){


    lineLeveldiff = 0;

    lineLeveldiff = QInputDialog::getInt(nullptr, "Depth Difference", "Layering difference in depth");
    lineLeveldiff*=-1;



    for (int i = 0; i < allShapesSampledPoints.size(); ++i) {

        int reconstType = 0;

        while (reconstType < 1 || reconstType > 2){
            reconstType = QInputDialog::getInt(nullptr, "Select Reconstruction Type for " + names[i], "Select Reconstruction Type for: " + names[i] + " \n 1 - Rotational Blending Surface \n 2 - Hermitian Radial Basis Function");
        }
        if (reconstType == 1) {
            RotationalBlendingSurface(i, paths[i], allShapesSampledPoints[i].ql,allShapesSampledPoints[i].qr);

        } else {
            DataForHRBF (i, paths[i], allShapesSampledPoints[i].ql,allShapesSampledPoints[i].qr);
        }
    }

    update();

}
