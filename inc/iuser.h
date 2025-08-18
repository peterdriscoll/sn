#pragma once

class IUser
{

public:
	virtual ~IUser() noexcept = default;
	virtual size_t GetNumThreads() const = 0;
};
