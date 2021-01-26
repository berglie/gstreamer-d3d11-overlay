using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace GStreamerD3D.Samples.WPF.D3D11
{
    public static class Utilities
    {
        private static readonly log4net.ILog log = log4net.LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);
        private static bool x86found = false;
        private static bool x64found = false;
        private static bool environmentPathWrong = false;
        private static char drive = 'A';

        private static List<string> possible86variables = new List<string> { "GSTREAMER_1_0_ROOT_MSVC_X86", "GSTREAMER_1_0_ROOT_MINGW_X86", "GSTREAMER_1_0_ROOT_X86" };
        private static List<string> possible64variables = new List<string> { "GSTREAMER_1_0_ROOT_MSVC_X86_64", "GSTREAMER_1_0_ROOT_MINGW_X86_64", "GSTREAMER_1_0_ROOT_X86_64" };

        private static List<string> possible86folderNames = new List<string> { "msvc_x86", "mingw_x86", "x86" };
        private static List<string> possible64folderNames = new List<string> { "msvc_x86_64", "mingw_x86_64", "x86_64" };


        public static void DetectGstPath()
        {
            bool x86found = false, x64found = false;

            log.Debug("Locating GStreamer path..");

            string installationPath = string.Empty;// Fallback path

            for (char c = drive; c <= 'Z'; c++)
            {
                if (Directory.Exists($@"{c}:\gstreamer"))
                {
                    installationPath = $@"{c}:\gstreamer";
                    drive = c;
                    break;
                }
            }

            string variable86 = possible86variables[0];

            for (int i = 0; i < possible86variables.Count; i++)
            {
                var environmentPath = Environment.GetEnvironmentVariable(possible86variables[i]);

                if (!String.IsNullOrEmpty(environmentPath))
                {
                    variable86 = possible86variables[i];
                    x86found = true;
                    if (IntPtr.Size == 4)
                    {
                        var envInstallationPath = System.IO.Path.Combine(environmentPath, "bin");
                        if (Directory.Exists(envInstallationPath))
                        {
                            installationPath = envInstallationPath;
                            environmentPathWrong = false;
                        }
                        else
                        {
                            log.Warn($"Installation path was not found in environment path ({envInstallationPath}), trying {installationPath} instead..");
                            environmentPathWrong = true;
                        }
                        log.Debug($"Using the 32-bit version of GStreamer ({variable86} - {installationPath})...");
                    }
                    break;
                }
            }

            string variable64 = possible86variables[0];

            for (int i = 0; i < possible64variables.Count; i++)
            {
                var environmentPath = Environment.GetEnvironmentVariable(possible64variables[i]);

                if (!String.IsNullOrEmpty(environmentPath))
                {
                    x64found = true;
                    variable64 = possible64variables[i];

                    if (IntPtr.Size == 8)
                    {
                        var envInstallationPath = System.IO.Path.Combine(environmentPath, "bin");
                        if (Directory.Exists(envInstallationPath))
                        {
                            installationPath = envInstallationPath;
                            environmentPathWrong = false;
                        }
                        else
                        {
                            log.Warn($"Installation path was not found in environment path ({envInstallationPath}), trying {installationPath} instead..");
                            environmentPathWrong = true;
                        }
                        log.Debug($"Using the 64-bit version of GStreamer ({variable64} - {installationPath})...");
                    }
                    break;
                }
            }

            if ((!x64found && IntPtr.Size == 8) || (!x86found && IntPtr.Size == 4) || environmentPathWrong)
            {
                string newPath = string.Empty;
                string newVariable = string.Empty;

                if (IntPtr.Size == 4)
                {
                    string folderName = possible86folderNames[0];

                    for (int i = 0; i < possible86folderNames.Count; i++)
                    {
                        if (Directory.Exists(Path.Combine(installationPath, "1.0", possible86folderNames[i])))
                        {
                            newPath = possible86folderNames[i];
                            newVariable = possible86variables.FirstOrDefault(v => v.ToLower().Contains(newPath.ToLower()));
                            break;
                        }
                    }
                }
                else if (IntPtr.Size == 8)
                {
                    string folderName = possible64folderNames[0];

                    for (int i = 0; i < possible64folderNames.Count; i++)
                    {
                        if (Directory.Exists(Path.Combine(installationPath, "1.0", possible64folderNames[i])))
                        {
                            newPath = possible64folderNames[i];
                            newVariable = possible64variables.FirstOrDefault(v => v.ToLower().Contains(newPath.ToLower()));
                            break;
                        }
                    }
                }

                newPath = Path.Combine(installationPath, "1.0", newPath);
                var binDir = Path.Combine(newPath, "bin");
                if (!Directory.Exists(binDir))
                {
                    log.Debug(binDir + " does not exist.");
                    if (++drive <= 'Z')
                    {
                        DetectGstPath();
                    }
                    else
                    {
                        log.Fatal($"Couldn't locate a GStreamer installation at {installationPath}. Please check your environment variable install GStreamer if missing.");
                        return;
                    }
                }

                if (String.IsNullOrEmpty(newVariable))
                {
                    log.Fatal($"Couldn't locate system environment variable for {(IntPtr.Size == 4 ? "x86" : "x64")} environment. ");
                    return;
                }

                log.Info($"Creating new environment variables ({newVariable} - {newPath})...");

                string newPluginpath = Path.Combine(newPath, "lib", "gstreamer-1.0"); // path to your gstream
                string registry = System.IO.Path.Combine(newPath, "registry.bin");

                Environment.SetEnvironmentVariable(newVariable, newPath);
                Environment.SetEnvironmentVariable("PATH", newPath + ";" + Environment.GetEnvironmentVariable("PATH"));
                Environment.SetEnvironmentVariable("GST_PLUGIN_PATH", newPluginpath + ";" + Environment.GetEnvironmentVariable("GST_PLUGIN_PATH"));
                Environment.SetEnvironmentVariable("GST_PLUGIN_SYSTEM_PATH_1_0", newPluginpath + ";" + Environment.GetEnvironmentVariable("GST_PLUGIN_SYSTEM_PATH_1_0"));
                Environment.SetEnvironmentVariable("GST_PLUGIN_SYSTEM_PATH", newPluginpath + ";" + Environment.GetEnvironmentVariable("GST_PLUGIN_SYSTEM_PATH"));
                Environment.SetEnvironmentVariable("GST_DEBUG", "*:4");
                Environment.SetEnvironmentVariable("GST_DEBUG_FILE", System.IO.Path.Combine(newPath, "gstreamer.log"));
                Environment.SetEnvironmentVariable("GST_REGISTRY", registry);
                DetectGstPath();
                return;
            }
            else if (!x64found && !x86found)
            {
                log.Fatal($"Couldn't locate a GStreamer installation. Please check your environment variable install GStreamer if missing.");
                return;
            }

            if (!System.IO.Directory.Exists(installationPath) || environmentPathWrong)
            {
                if (++drive <= 'Z')
                {
                    DetectGstPath();
                }
                else
                {
                    log.Fatal($"Couldn't locate a GStreamer installation at {installationPath}. Please check your environment variable install GStreamer if missing.");
                    return;
                }

                log.Fatal($"Couldn't locate a GStreamer installation at {installationPath}. Please check your environment variable install GStreamer if missing.");
                return;
            }

            var path = Environment.GetEnvironmentVariable("Path");

            // GStreamer uses the Path variable to find itself. 
            if (!path.StartsWith(installationPath))
                Environment.SetEnvironmentVariable("Path", installationPath + ";" + path);
        }

        public static bool IsPathDetected()
        {
            return x86found || x64found;
        }
    }
}
