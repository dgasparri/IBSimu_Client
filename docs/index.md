# IBSimu Client

IBSimu Client is a client application for the [IBSimu Ion Beam Simulator](http://ibsimu.sourceforge.net/) library for plasma simulation. It is designed to run on both Windows and Linux.

This client application was developed because I was tired of recompiling the code every time I wanted to change a parameter of the simulation. The IBSimu library is the workhorse that does all the heavy lifting job, and this client can do nothing more than the original library can do in terms of physics. 

The client is just a convenient way to:

* keep all simulation parameters (geometry, boundary conditions, beam composition) in a clean, human-readable text file
* change simulation parameters without recompiling the code
* run reasonably sized simulations
* run simulations on remote/cloud machines such as Amazon EC2
* collect beam data during the simulation, including Twiss parameters, potentials and particle trajectories
* visualize the results of previous run simulations

# Installation

The client can run on both Windows and Linux. If you are not interested in the details, you can find a straightforward [list of steps to install it on a Ubuntu machine](install-amazon-EC2), either local or cloud based.

Before installing the client, you have to install IBSimu and all of its dependencies on Linux or on Windows. Here is a list of packages:

* pkg-config
* fontconfig 
* freetype2-demos
* libcairo2-dev
* libpng-dev
* zlib1g zlib1g-dev
* libpthread-stubs0-dev
* libgsl-dev
* libgtk-3-dev
* libgtkglext1-dev
* libumfpack5
* libsuitesparse-dev
* gcc-10 gcc-10-base gcc-10-doc  - or higher
* g++-10 - or higher
* libstdc++-10-dev libstdc++-10-doc  - or higher


The client also requires the Boost C++ library.







Latex trial 1:

\[\frac{dx}{dt} \]

Latex trial 2

\\[\frac{dx}{dt} \\]

Latex trial 1:

$$\frac{dx}{dt}$$

Latex trial 1:

{% raw %}

\[\frac{dx}{dt} \]

{% endraw %}

End text
