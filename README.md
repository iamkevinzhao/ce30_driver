This page is set up for CE30-D SDK technical support. For product information about CE30 series and other Benewake products, please visit our [offical website](http://www.benewake.com/en/index.html).

---

#### Version

- v1.0.4

#### Get Started

CE30-D adopts the User Dategram Protocol (UDP) to interchange data messages with host machines. We highly recommend users coding their own program for interacting with CE30-D. In such a case, you can proceed to our [offical website](http://www.benewake.com/en/index.html) for the documentation of our UDP communication stipulations. However, if you wish to quickly deploy CE30-D on your application platforms and test its functionalities, then this repository will help you. Here, we provide a C++ library that helps you to retrieve CE30-D measurement data at ease. The library is able to be compiled and run on both Windows and Linux with few dependencies (mainly the Boost Library). However, we develop and test this library on 64-bit Windows 10 and Ubuntu 16.04 x86_64 with Intel Core i7 and 16G memory for most of the time. Thus, we expect your computer to match this configuration as much as possible.