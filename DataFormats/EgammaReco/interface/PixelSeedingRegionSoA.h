#ifndef DataFormats_EgammaReco_interface_PixelSeedingRegionSoA_h
#define DataFormats_EgammaReco_interface_PixelSeedingRegionSoA_h

#include <Eigen/Core>
#include <cstdint>
#include "DataFormats/SoATemplate/interface/SoACommon.h"
#include "DataFormats/SoATemplate/interface/SoALayout.h"


namespace reco {

	using Vector3d = Eigen::Matrix<double, 3, 1>;
	using Matrix4d = Eigen::Matrix<double, 4, 4>;

	GENERATE_SOA_LAYOUT(PixelSeedingRegionLayout,
        SOA_SCALAR(bool, isTriplet),
        SOA_SCALAR(uint32_t, size),
		SOA_SCALAR(float, ptMin),
		SOA_SCALAR(float, vertexRBound),
		SOA_SCALAR(float, vertexZBound),
		SOA_SCALAR(float, tanLambdaRangeMin),
		SOA_SCALAR(float, tanLambdaRangeMax),
        SOA_SCALAR(float, phiMarginLeft),
		SOA_SCALAR(float, phiMarginRight),
		SOA_SCALAR(float, directionX),
		SOA_SCALAR(float, directionY),
		SOA_SCALAR(float, directionZ),
		SOA_SCALAR(float, vertexPosX),
		SOA_SCALAR(float, vertexPosY),
		SOA_SCALAR(float, vertexPosZ),
		SOA_COLUMN(uint32_t, hit0Detid),
		SOA_EIGEN_COLUMN(Vector3d, hit0LocalPos),
		SOA_EIGEN_COLUMN(Vector3d, hit0LocalPosError),		// xx, xy, yy, see DataFormats/GeometrySurface/interface/LocalError.h
		SOA_EIGEN_COLUMN(Vector3d, hit0GlobalPos),
		SOA_EIGEN_COLUMN(Matrix4d, hit0GlobalPosError),		// see DataFormats/GeometryCommonDetAlgo/interface/GlobalErrorBase.h
		SOA_COLUMN(uint32_t, hit1Detid),
		SOA_EIGEN_COLUMN(Vector3d, hit1LocalPos),
		SOA_EIGEN_COLUMN(Vector3d, hit1LocalPosError),
		SOA_EIGEN_COLUMN(Vector3d, hit1GlobalPos),
		SOA_EIGEN_COLUMN(Matrix4d, hit1GlobalPosError),
		SOA_COLUMN(uint32_t, hit2Detid),
		SOA_EIGEN_COLUMN(Vector3d, hit2LocalPos),
		SOA_EIGEN_COLUMN(Vector3d, hit2LocalPosError),
		SOA_EIGEN_COLUMN(Vector3d, hit2GlobalPos),
		SOA_EIGEN_COLUMN(Matrix4d, hit2GlobalPosError),

	)
	using PixelSeedingRegionSoA = PixelSeedingRegionLayout<>;
}  // namespace reco

#endif
