#include "libuds.h"
#include "misc/Vehicle.h"

std::shared_ptr<uds::Base> uds::autoReleaseNew(std::shared_ptr<can::Base> can, int channel, 
	int proto, int dlc, CommunicationId cid, GetKeyCallback callback)
{
	return std::make_shared<uds::Base>(can, channel, proto, dlc, cid, callback);
}

std::shared_ptr<uds::Base> uds::autoReleaseNew(uds::VehicleType type,
	std::shared_ptr<can::Base> can, int channel, int proto, int dlc)
{
	std::shared_ptr<uds::Base> base = nullptr;
	switch (type)
	{
	case uds::VT_SGMW_201S_AVM:
		base = std::make_shared<uds::Sgmw201sAvm>(can, channel, proto, dlc);
		break;
	default:
		break;
	}
	return base;
}

