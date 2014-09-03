copy ViewBar\Resource.h Lang\EN\ViewBar
copy ViewBar\ViewBar.rc Lang\EN\ViewBar
copy ViewBar\res\VersionNo.h Lang\EN\ViewBar\res
copy ViewBar\res\ViewBar.rc2 Lang\EN\ViewBar\res

copy ViewBar\Resource.h Lang\ZH\ViewBar
copy ViewBar\res\VersionNo.h Lang\ZH\ViewBar\res
copy ViewBar\res\ViewBar.rc2 Lang\ZH\ViewBar\res

"%VS80_HOME%\Common7\IDE\devenv.exe"  Lang\EN\ViewBar\ViewBar.sln /build Release
"%VS80_HOME%\Common7\IDE\devenv.exe"  Lang\ZH\ViewBar\ViewBar.sln /build Release