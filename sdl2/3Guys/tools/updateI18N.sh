#!/usr/bin/php

<?php

/*
Copyright (C) 2015-2018 Parallel Realities

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

$strings = [];

function addString($string)
{
	global $strings;

	if ($string != "")
	{
		if (!in_array($string, $strings))
		{
			$strings[] = $string;
		}
	}
}

function extractC($filename)
{
	$i = 0;

	$lines = file($filename);
	
	$reg = "(_\\(\"([^\\\"]*)\")";
	
	foreach ($lines as $line)
	{
		$i++;
	
		if (preg_match($reg, $line, $matches) > 0)
		{
			addString($matches[1]);
		}
	}
}

function extractJSON($filename)
{
	$data = file_get_contents($filename);
	$json = json_decode($data);
	
	if (strpos($filename, "widget") !== false)
	{
		foreach ($json as $widget)
		{
			if (array_key_exists("label", $widget))
			{
				addString($widget->{"label"}, $filename);
			}
		}
	}
	else if (strpos($filename, "levels") !== false)
	{
		$meta = $json->{"meta"};
		
		if (array_key_exists("tips", $meta))
		{
			foreach ($meta->{"tips"} as $tip)
			{
				addString($tip);
			}
		}
	}
}

function recurseDir($dir)
{
	$files = array_diff(scandir($dir), array('..', '.'));
	
	foreach ($files as $file)
	{
		if (is_dir("$dir/$file"))
		{
			recurseDir("$dir/$file");
		}
		else if (strstr($file, ".c") !== FALSE)
		{
			extractC("$dir/$file");
		}
		else if (strstr($file, ".json") !== FALSE)
		{
			extractJSON("$dir/$file");
		}
	}
}

recurseDir("../src");

recurseDir("../data/widgets");

recurseDir("../data/levels");

$potHeader = file_get_contents("../tools/potHeader.txt");

$handle = fopen("../locale/3Guys.pot", "w");

$dateTime = date("Y-m-d H:i:sO");

$potHeader = str_replace("{POT_CREATION_DATE}", $dateTime, $potHeader);

fwrite($handle, "$potHeader\n");

foreach ($strings as $string)
{
	fwrite($handle, "msgid \"$string\"\n");
	fwrite($handle, "msgstr \"\"\n");
	fwrite($handle, "\n");
}

fclose($handle);

?>
