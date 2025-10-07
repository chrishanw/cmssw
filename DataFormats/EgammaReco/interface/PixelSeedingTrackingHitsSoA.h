#ifndef DataFormats_EgammaReco_interface_PixelSeedingTrackingHitsSoA_h
#define DataFormats_EgammaReco_interface_PixelSeedingTrackingHitsSoA_h

#include <Eigen/Core>
#include <cstdint>
#include "DataFormats/SoATemplate/interface/SoACommon.h"
#include "DataFormats/SoATemplate/interface/SoALayout.h"


namespace reco {

	using Vector3d = Eigen::Matrix<double, 3, 1>;
	using Matrix4d = Eigen::Matrix<double, 4, 4>;

	GENERATE_SOA_LAYOUT(PixelSeedingTrackingHitsLayout,
		SOA_SCALAR(uint32_t, size),
		SOA_COLUMN(uint32_t, detid),
		SOA_EIGEN_COLUMN(Vector3d, localPos),
		SOA_EIGEN_COLUMN(Vector3d, localPosError),		// xx, xy, yy, see DataFormats/GeometrySurface/interface/LocalError.h
		SOA_EIGEN_COLUMN(Vector3d, globalPos),
		SOA_EIGEN_COLUMN(Matrix4d, globalPosError)		// see DataFormats/GeometryCommonDetAlgo/interface/GlobalErrorBase.h
	)
	using PixelSeedingTrackingHitsSoA = PixelSeedingTrackingHitsLayout<>;
}  // namespace reco

#endif
