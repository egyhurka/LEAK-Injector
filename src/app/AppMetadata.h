#pragma once

namespace leak::app
{
	struct AppMetadata
	{
		const char* build;
	};

	inline constexpr AppMetadata Metadata{
		"v0.1.0-beta"
	};
}
