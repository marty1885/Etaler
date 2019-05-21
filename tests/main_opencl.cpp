#define CATCH_CONFIG_RUNNER
#include "catch2/catch.hpp"


#include <Etaler/Backends/OpenCLBackend.hpp>
#include <Etaler/Core/DefaultBackend.hpp>

int main( int argc, char* argv[] )
{
	auto backend = std::make_shared<et::OpenCLBackend>();
	et::setDefaultBackend(backend.get());

	std::cout << "Running with backend: " << et::defaultBackend()->name() << std::endl;

	int result = Catch::Session().run( argc, argv );

	return result;
}
