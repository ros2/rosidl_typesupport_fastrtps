^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package rosidl_typesupport_fastrtps_c
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Forthcoming
-----------
* Support Fast CDR v2 (`#114 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/114>`_)
* Improve wide string (de)serialization (`#113 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/113>`_)
* Set hints to find the python version we actually want. (`#112 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/112>`_)
  Co-authored-by: Michael Carroll <michael@openrobotics.org>
* Contributors: Chris Lalancette, Miguel Company

3.4.0 (2023-12-26)
------------------
* Update to C++17 (`#111 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/111>`_)
* Contributors: Chris Lalancette

3.3.0 (2023-10-04)
------------------
* Account for alignment on `is_plain` calculations (`#108 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/108>`_)
* Contributors: Miguel Company

3.2.0 (2023-06-07)
------------------

3.1.0 (2023-04-28)
------------------

3.0.0 (2023-04-12)
------------------
* Type Description Nested Support (`#101 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/101>`_)
* Type hashes on typesupport (rep2011) (`#98 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/98>`_)
* Expose type hash to typesupport structs (rep2011) (`#95 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/95>`_)
* Mark benchmark _ as UNUSED. (`#96 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/96>`_)
* Contributors: Chris Lalancette, Emerson Knapp

2.5.0 (2023-02-13)
------------------
* Service introspection (`#92 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/92>`_)
* Update rosidl_typesupport_fastrtps to C++17. (`#94 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/94>`_)
* [rolling] Update maintainers - 2022-11-07 (`#93 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/93>`_)
* Contributors: Audrow Nash, Brian, Chris Lalancette

2.4.0 (2022-09-13)
------------------
* Replace rosidl_cmake imports with rosidl_pycommon (`#91 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/91>`_)
* Contributors: Jacob Perron

2.3.0 (2022-05-04)
------------------

2.2.0 (2022-03-30)
------------------
* Install generated headers to include/${PROJECT_NAME} (`#88 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/88>`_)
* Misc fastrtps typesupport generator cleanup (`#87 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/87>`_)
* Contributors: Shane Loretz

2.1.0 (2022-03-01)
------------------
* Install headers to include/${PROJECT_NAME} (`#86 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/86>`_)
* Contributors: Shane Loretz

2.0.4 (2022-01-13)
------------------
* Fix include order for cpplint (`#84 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/84>`_)
* Update maintainers to Shane Loretz (`#83 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/83>`_)
* Contributors: Audrow Nash, Jacob Perron

2.0.3 (2021-11-18)
------------------
* Use FindPython3 explicitly instead of PythonInterp implicitly (`#78 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/78>`_)
* Contributors: Shane Loretz

2.0.2 (2021-08-09)
------------------
* Revert rosidl targets and dependencies exportation (`#76 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/76>`_)
  * Revert "Export rosidl_typesupport_fastrtps_c* dependencies (`#75 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/75>`_)"
  * Revert "Bundle and ensure the exportation of rosidl generated targets (`#73 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/73>`_)"
* Correctly inform that a BoundedSequence is bounded (`#71 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/71>`_)
* Contributors: Michel Hidalgo, Miguel Company

2.0.1 (2021-07-28)
------------------
* Export rosidl_typesupport_fastrtps_c* dependencies (`#75 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/75>`_)
* Contributors: Michel Hidalgo

2.0.0 (2021-07-23)
------------------
* Bundle and ensure the exportation of rosidl generated targets (`#73 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/73>`_)
* Fix Fast-RTPS C++ typesupport CLI extension (`#72 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/72>`_)
* Fastdds type support extensions (`#67 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/67>`_)
* Remove fastrtps dependency (`#68 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/68>`_)
* Contributors: Andrea Sorbini, Michel Hidalgo, Miguel Company

1.2.1 (2021-04-06)
------------------
* updating quality declaration links (re: `ros2/docs.ros2.org#52 <https://github.com/ros2/docs.ros2.org/issues/52>`_) (`#69 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/69>`_)
* Contributors: shonigmann

1.2.0 (2021-03-18)
------------------
* Expose FastRTPS C typesupport generation via rosidl generate CLI (`#65 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/65>`_)
* Contributors: Michel Hidalgo

1.1.0 (2020-12-09)
------------------
* Update QDs with up-to-date content (`#64 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/64>`_)
* Fix item number in QD (`#59 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/59>`_)
* Update QL to 2
* Update package maintainers (`#55 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/55>`_)
* Updat QD (`#53 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/53>`_)
* Fix invalid return on deserialize function (`#51 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/51>`_)
* Added benchmark test to rosidl_typesupport_fastrtps_c/cpp (`#52 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/52>`_)
* Update exec dependencies (`#50 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/50>`_)
* Add Security Vulnerability Policy pointing to REP-2006 (`#44 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/44>`_)
* QD Update Version Stability to stable version (`#46 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/46>`_)
* Contributors: Alejandro Hernández Cordero, Chris Lalancette, Jorge Perez, Louise Poubel, Michel Hidalgo, Stephen Brawner, sung-goo-kim

1.0.1 (2020-05-26)
------------------
* Revert usage of modern cmake. This breaks single typesupport builds again. (`#47 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/47>`_)
* Contributors: Ivan Santiago Paunovic

1.0.0 (2020-05-22)
------------------
* Use modern cmake to fix single typesupport builds (`#40 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/40>`_)
* Move generated headers to detail subdir (`#40 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/40>`_)
* Add tests for wstring conversion routines (`#43 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/43>`_
* Update public API documentation (`#42 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/42>`_)
* Add feature documentation (`#41 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/41>`_)
* Add Quality Declaration and README (`#39 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/39>`_)
* Contributors: Ivan Santiago Paunovic, Scott K Logan, brawner

0.9.0 (2020-04-24)
------------------
* Export targets in addition to include directories / libraries (`#37 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/37>`_)
* Update includes to use non-entry point headers from detail subdirectory (`#36 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/36>`_)
* Use ament_cmake_ros (`#30 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/30>`_)
* Rename rosidl_generator_c namespace to rosidl_runtime_c (`#35 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/35>`_)
* Added rosidl_runtime_c depencency (`#32 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/32>`_)
* Export typesupport library in a separate cmake variable (`#34 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/34>`_)
* Style update to match uncrustify with explicit language (`#31 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/31>`_)
* Code style only: wrap after open parenthesis if not in one line (`#29 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/29>`_)
* Contributors: Alejandro Hernández Cordero, Dirk Thomas, Ivan Santiago Paunovic

0.8.0 (2019-09-25)
------------------
* Remove non-package from ament_target_dependencies() (`#27 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/27>`_)
* Fix typesupport for long double and wchar (`#26 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/26>`_)
* Contributors: Dirk Thomas, Shane Loretz

0.7.1 (2019-05-08)
------------------
* Add message namespace to type support struct (`#18 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/18>`_)
* Hard code size of wchar_t to 4 (`#25 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/25>`_)
* Fix size calculation for WStrings on non-Windows platforms (`#23 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/23>`_)
* Ensure boolean initialization in FastRTPS (`#24 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/24>`_)
* Add WString support (`#22 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/22>`_)
* Simplify code using updated definition API (`#21 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/21>`_)
* Update code to match refactoring of rosidl definitions (`#20 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/20>`_)
* Remove usage of UnknownMessageType (`#19 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/19>`_)
* Contributors: Dirk Thomas, Jacob Perron, Karsten Knese, Michael Carroll

0.7.0 (2019-04-13)
------------------
* Change generators to IDL-based pipeline (`#14 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/14>`_)
* Contributors: Dirk Thomas

0.6.1 (2019-01-11)
------------------
* Change uncrustify max line length to 0 (`#17 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/17>`_)
  This is for compatibility with uncrustify v0.68.
* Updated message to say fastrtps instead of Connext (`#16 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/16>`_)
* Contributors: Jacob Perron, Johnny Willemsen

0.6.0 (2018-11-16)
------------------
* Allow generated IDL files (`#12 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/12>`_)
* Rename dynamic array to sequence (`#13 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/13>`_)
* Enable generation of messages and services in an 'action' directory (`#11 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/11>`_)
* Remove unnecessary dll exports (`#8 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/8>`_)
* Fix the target dependency for automatic regeneration (`#7 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/7>`_)
* Avoid using undefined variable (`#5 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/5>`_)
* Remove more dead code (`#4 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/4>`_)
* Don't generate IDL files and remove unused code (`#2 <https://github.com/ros2/rosidl_typesupport_fastrtps/issues/2>`_)
* Contributors: Alexis Pojomovsky, Dirk Thomas, Michel Hidalgo, Miguel Company, Mikael Arguedas, Shane Loretz
