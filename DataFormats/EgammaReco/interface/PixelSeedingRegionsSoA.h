#ifndef DataFormats_EgammaReco_interface_PixelSeedingRegionsSoA_h
#define DataFormats_EgammaReco_interface_PixelSeedingRegionsSoA_h

#include <Eigen/Core>
#include <cstdint>
#include "DataFormats/SoATemplate/interface/SoACommon.h"
#include "DataFormats/SoATemplate/interface/SoALayout.h"


namespace reco {

	using Vector3d = Eigen::Matrix<double, 3, 1>;

	GENERATE_SOA_LAYOUT(PixelSeedingRegionsLayout,
		SOA_SCALAR(uint32_t, size),
		SOA_COLUMN(float, ptMin),
		SOA_COLUMN(float, vertexRBound),
		SOA_COLUMN(float, vertexZBound),
		SOA_COLUMN(float, tanLambdaRangeMin),
		SOA_COLUMN(float, tanLambdaRangeMax),
        SOA_COLUMN(float, phiMarginLeft),
		SOA_COLUMN(float, phiMarginRight),
		SOA_EIGEN_COLUMN(Vector3d, direction),
		SOA_EIGEN_COLUMN(Vector3d, vertexPos)
	)
	using PixelSeedingRegionsSoA = PixelSeedingRegionsLayout<>;
}  // namespace reco

#endif
