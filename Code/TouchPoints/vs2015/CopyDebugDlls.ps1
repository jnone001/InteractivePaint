$debugFolder = ".\Debug\"

if (!(Test-Path -path $debugFolder)) {New-Item $debugFolder -Type Directory}

Copy-Item .\dependencies\LeapSDK\lib\x86\Leap.dll .\Debug\ -Force -Verbose
Copy-Item .\dependencies\libusb-1.0.20\MS32\dll\libusb-1.0.dll .\Debug\ -Force -Verbose
Copy-Item .\dependencies\TobiiEyeXSDK_1.7.480\lib\x86\Tobii.EyeX.Client.dll .\Debug\ -Force -Verbose