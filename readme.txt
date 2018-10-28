1.LUT excel is input for creating the station list to use in Multi FM.
2.Four columns are defined in total: 
	a. 1st two column has country & country code, 3rd column has PI code & 4th column has PSN.
3.Multi FM version is not a factor.
4.1st input : only country code entered from the drop down menu for selection
5.2nd input: Number of channels needed.
6.Scripter scans the first 'n' entry based on user information and xx region code.
	a. The scanned entries are inputs for the multi fm replication.
	b. Ini file is used for accessing the basic setting template

	also do
	$wgGenerateThumbnailOnParse = true;
	$wgMaxShellMemory = 524288;
	$wgMaxShellTime   = ;
	$wgUseInstantCommons  = false;
	$wgMaxShellFileSize = 524288;