#pragma once
#include "Base.h"

namespace uds {
	class UDS_DLL_EXPORT Sgmw201sAvm : public Base
	{
	public:
		Sgmw201sAvm(std::shared_ptr<can::Base> base, int channel, int proto, int dlc);
		~Sgmw201sAvm();

	private:
		bool getKey(int level, const unsigned char* seedData, int seedSize, unsigned char* keyData, int* keySize = nullptr) override;
	};
}
