# Introduction #

This page explains how to start working on the Flex visualization applet included in Neologism.

**_Note: This is somewhat outdated!_**

# Requirements #

[Adobe Flex Builder 3](http://www.adobe.com/products/flex/)

The following ActionScript 3 libraries are used:
  * [Object Handles](http://www.rogue-development.com/objectHandles.html)
  * [Tweener](http://code.google.com/p/tweener/)
  * [Distortion effects](http://weblogs.macromedia.com/auhlmann/archives/effects/index.html)


# For developers #

To contribute to the NeosVis vocabulary visualization applet, the following steps are required:

  * Check out the current version from svn; NeosVis is located in the **/flex-src** folder
  * Create a new project in FlexBuilder 3.
  * Copy the contents of the checked out **/flex-src** folder to the newly created **/src** folder in your local FlexBuilder 3 project.
  * In FlexBuider 3 go to _Project_ -> _Properties_, in the _Flex Applications_ tab, add the **Neologism.mxml** application and set it as default.

By now everything should be running ok.

In case you get the following Flex error: **SecurityError: Error #2148**, go to _Project_ -> _Properties_, in the _Flex Compiler_ tab add the following **-use-network=false** in the _Additional compiler arguments_ field.
Please note that by adding the mentioned compiler argument, you disable networking in flex, when deploying the applet (_Neologism.swf_) remove the flag before compiling.