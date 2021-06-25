# Add project specific ProGuard rules here.
# By default, the flags in this file are appended to flags specified
# in /opt/android/android-sdk/tools/proguard/proguard-android.txt
# You can edit the include path and order by changing the proguardFiles
# directive in build.gradle.
#
# For more details, see
#   http://developer.android.com/guide/developing/tools/proguard.html

# Add any project specific keep options here:

# If your project uses WebView with JS, uncomment the following
# and specify the fully qualified class name to the JavaScript interface
# class:
#-keepclassmembers class fqcn.of.javascript.interface.for.webview {
#   public *;
#}
#---------------------------------基本指令区----------------------------------
-optimizationpasses 5
-dontusemixedcaseclassnames
-dontskipnonpubliclibraryclasses
-dontskipnonpubliclibraryclassmembers
-dontpreverify
-verbose
-printmapping proguardMapping.txt
-optimizations !code/simplification/cast,!field/*,!class/merging/*
-keepattributes *Annotation*,InnerClasses
-keepattributes Signature
-keepattributes SourceFile,LineNumberTable
#----------------------------------------------------------------------------

#---------------------------------默认保留区---------------------------------
-keep public class * extends android.app.Activity
-keep public class * extends android.app.Application
-keep public class * extends android.app.Service
-keep public class * extends android.content.BroadcastReceiver
-keep public class * extends android.content.ContentProvider
-keep public class * extends android.app.backup.BackupAgentHelper
-keep public class * extends android.preference.Preference
-keep public class * extends android.view.View
-keep public class com.android.vending.licensing.ILicensingService
-keep class android.support.** {*;}

-keepclasseswithmembernames class * {
    native <methods>;
}
-keepclassmembers class * extends android.app.Activity{
    public void *(android.view.View);
}
-keepclassmembers enum * {
    public static **[] values();
    public static ** valueOf(java.lang.String);
}
-keep public class * extends android.view.View{
    *** get*();
    void set*(***);
    public <init>(android.content.Context);
    public <init>(android.content.Context, android.util.AttributeSet);
    public <init>(android.content.Context, android.util.AttributeSet, int);
}
-keepclasseswithmembers class * {
    public <init>(android.content.Context, android.util.AttributeSet);
    public <init>(android.content.Context, android.util.AttributeSet, int);
}
-keep class * implements android.os.Parcelable {*;}

-keepclassmembers class * implements java.io.Serializable {
    static final long serialVersionUID;
    private static final java.io.ObjectStreamField[] serialPersistentFields;
    private void writeObject(java.io.ObjectOutputStream);
    private void readObject(java.io.ObjectInputStream);
    java.lang.Object writeReplace();
    java.lang.Object readResolve();
}
-keep class **.R$* {
 *;
}

-keep class * extends java.lang.annotation.Annotation {*;}
-keep class * implements javax.annotation.processing.Processor {*;}
-dontwarn javax.annotation.processing.AbstractProcessor
-keep class javax.annotation.processing.AbstractProcessor {*;}

-keepattributes Signature
-keepattributes EnclosingMethod

#FoundationLibrary
#qdaccess
-keep class com.qdingnet.qdaccess.ProtocolHelper { *; }
-keep class com.qdingnet.qdaccess.QDAccessMsgHandler { *; }
-keep class com.qdingnet.qdaccess.QDConfigMsgHandler { *; }
-keep class com.qdingnet.qdaccess.QDDeviceConfigEntity { *; }
-keep class com.qdingnet.qdaccess.QDPassRecordEntity { *; }
-keep class com.qdingnet.qdaccess.QDRFCardItem { *; }
-keep class com.qdingnet.qdaccess.QDUnitItem { *; }

#ConfigSdkLibrary
#config/bean
-keep class com.qdingnet.provider.config.bean.DeviceConfigMsg { *; }
-keep class com.qdingnet.provider.config.bean.GateBuildUnitInfo { *; }
-keep class com.qdingnet.provider.config.bean.QDAccessResult { *; }
-keep class com.qdingnet.provider.config.bean.ScanResult { *; }
#config/bundle
-keep class com.qdingnet.provider.config.bundle.OpenDoorBundle { *; }
-keep class com.qdingnet.provider.config.bundle.OpenDoorBundle$OpenDoorBundleBuilder { *; }
-keep class com.qdingnet.provider.config.bundle.WriteConfigBundle { *; }
-keep class com.qdingnet.provider.config.bundle.WriteConfigBundle$Builder { *; }
-keep class com.qdingnet.provider.config.bundle.ReadConfigBundle { *; }
-keep class com.qdingnet.provider.config.bundle.ReadConfigBundle$Builder { *; }
-keep class com.qdingnet.provider.config.bundle.ReadVersionBundle { *; }
-keep class com.qdingnet.provider.config.bundle.ReadVersionBundle$Builder { *; }
-keep class com.qdingnet.provider.config.bundle.DeviceUpgradeBundle { *; }
-keep class com.qdingnet.provider.config.bundle.DeviceUpgradeBundle$Builder { *; }
#config/callback
-keep class com.qdingnet.provider.config.callback.IOpenDoorCallback { *; }
-keep class com.qdingnet.provider.config.callback.IScanDeviceCallback { *; }
-keep class com.qdingnet.provider.config.callback.IWriteConfigCallback { *; }
-keep class com.qdingnet.provider.config.callback.IReadConfigCallback { *; }
-keep class com.qdingnet.provider.config.callback.IReadVersionCallback { *; }
-keep class com.qdingnet.provider.config.callback.IDeviceUpgradeCallback { *; }
#config/secure
-keep class com.qdingnet.provider.config.secure.SecureProvider { *; }
#config
-keep class com.qdingnet.provider.config.Logdeal { *; }
-keep class com.qdingnet.provider.config.ConfigManager { *; }

