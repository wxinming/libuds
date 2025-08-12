#include "libuds.h"
#include "misc/Vehicle.h"

std::shared_ptr<uds::Base> uds::autoReleaseNew(std::shared_ptr<can::Base> can, int channel, 
	can::ProtoType proto, CommunicationId cid, GetKeyCallback callback)
{
	return std::make_shared<uds::Base>(can, channel, proto, cid, callback);
}

std::shared_ptr<uds::Base> uds::autoReleaseNew(uds::VehicleType type,
	std::shared_ptr<can::Base> can, int channel, can::ProtoType proto)
{
	std::shared_ptr<uds::Base> base = nullptr;
	switch (type)
	{
	case uds::SGMW_201S_AVM:
		base = std::make_shared<uds::Sgmw201sAvm>(can, channel, proto);
		break;
	default:
		break;
	}
	return base;
}

