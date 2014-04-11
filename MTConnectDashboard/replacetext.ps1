# http://www.guyellisrocks.com/2008/06/powershell-replace-text-in-files-and.html

function ReplaceText($fileInfo)
{
    if( $_.GetType().Name -ne 'FileInfo')
    {
        # i.e. reject DirectoryInfo and other types
         return
    }
    $old = 'MtConnectDashboard'
    $new = 'MtConnectDashboard'
    (Get-Content $fileInfo.FullName) | % {$_ -replace $old, $new} | Set-Content -path $fileInfo.FullName
    "Processed: " + $fileInfo.FullName
}

$loc = 'C:\Users\michalos\Documents\GitHub\MTConnectToolbox\MTConnectDashboard'
cd $loc
$files = Get-ChildItem . -recurse

$files | % { ReplaceText( $_ ) }

