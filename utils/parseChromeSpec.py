#!/usr/bin/python

import io
import sys
import re

import urllib2

from bs4 import BeautifulSoup

import json

from optparse import OptionParser

parser = OptionParser();
parser.add_option("-o", "--output", default=None, dest="output", help="write result to a file", metavar="FILENAME");
parser.add_option("-u", "--url", default=None, dest="url", help="read input from url", metavar="URL");
parser.add_option("-v", "--verbose", default=False, action="store_true", dest="verbose");

(options, args) = parser.parse_args();

output = sys.stdout;
try:
	if (options.url):
		f = urllib2.urlopen(options.url);
	else:
		f = io.open(args[0]);

	if (options.output):
		output = io.open(options.output, 'w');

except Exception as e:
	sys.stderr.write(str(e) + "\n\n");
	parser.print_help();
	exit(1);


#f = urllib2.urlopen('http://developer.chrome.com/extensions/browserAction.html');
#f = io.open('/home/johny/tmp/browserAction.html');

#f = urllib2.urlopen('http://developer.chrome.com/extensions/tabs.html');
#f = io.open('/home/johny/tmp/tabs.html');
content = f.read();
f.close();

soup = BeautifulSoup(content);

def debugPrint(*arg):
	if (options.verbose):
		for i in arg:
			print str(i),
		print;

def processTypeDescription(aTypeString):
	types = re.split(" or ", aTypeString);
	
	typeSpecs = [];
	for i in types: 
		strings = i.split();
		if (len(strings) > 2):
			if (strings[0] == "array" and strings[1] == 'of'):
				typeSpecs.append({ "type" : "array", "items" : strings[2] });
			elif (strings[0] == "enumerated" and strings[1] == 'string'):
				typeSpecs.append({ "type" : "enumerated string", "enum" : json.loads(re.sub("enumerated string ", "", i)) });
		else:
			typeSpecs.append(i);
	if (len(typeSpecs) == 1):
		return typeSpecs[0];
	else:
		return typeSpecs;

def processArgument(aArgument, aDD, aIndent):
	argName = aArgument.find("span", "variable");
	argProperty = aArgument.find("span", "property");
	argType = argProperty.find("span", id="typeTemplate");
	optional = argProperty.find("span","optional") != None;
		
	debugPrint("\t" * aIndent, argName.text, argType.text, optional);
	if (aDD):
		dl = aDD.find("dl");
		if (dl != None):
			dts = dl.find_all("dt");
			for i in dts:
				processArgument(i,i.findNextSibling("dd"),aIndent+1);
	propertyDescription = { "id" : argName.text, "type" : argType.text, "required" : (not optional) }
	typeDescription = processTypeDescription(argType.text);
	if (typeDescription):
		propertyDescription["type"] = typeDescription;
	return propertyDescription;

def processMethod(aMethodDiv):
	h4 = aMethodDiv.find_all("h4");
	debugPrint(h4[0].text);
	debugPrint(' '.join((aMethodDiv.find("div", ["summary lower", "summary"]).text).split()));
	dl = aMethodDiv.find("dl");
	divs = dl.find_all("div", recursive=False);
	items = [];
	for i in divs:
		items.append(processArgument(i.find("dt",recursive=False),i.find("dd",recursive=False), 1));
	methodDescription = { "id" : h4[0].text, "type" : "functionArguments", "items" : items }
	return methodDescription;



def processType(aTypeDiv):
	h3 = aTypeDiv.find("h3", recursive=False);
	div = aTypeDiv.find("div", recursive=False);
	dt = div.find("dt", recursive=False);
	typeName = dt.find("span", id="typeTemplate");
	debugPrint(h3.text, typeName.text);

	dds = div.find_all("dd", recursive=False);
	dl = dds[-1].find("dl", recursive=False);
	divs = dl.find_all("div", recursive=False);
	properties = {};
	for i in divs:
		prop = processArgument(i.find("dt",recursive=False),i.find("dd",recursive=False), 1);
		properties[prop["id"]] = prop;
	typeDescription = { "id" : h3.text, "type" : typeName.text, "properties" : properties }
	return typeDescription;


title = soup.find("h1", "page_title");

typesHeadline = soup.find(id="types");
types = typesHeadline.find_next_siblings();
typeDescriptions = [];
for i in types:
	if (i.name == "div"):
		typeDescriptions.append(processType(i));
	else:
		break;



methodsHeadline = soup.find(id="methods");
methods = methodsHeadline.find_next_siblings();
methodDescriptions = [];
for i in methods:
	if (i.name == "div"):
		methodDescriptions.append(processMethod(i));
	else:
		break;


output.write(u"var apiName = \'%s\';\n\n\n" % title.text);

output.write(u"//" + (u"-" * 60) + u"\n");
output.write(u"//          Types extracted for %s\n" % title.text);
output.write(u"//" + (u"-" * 60) + u"\n");
output.write(u"var types = " + json.dumps(typeDescriptions, sort_keys=True, indent=2));
output.write(u"\n\n");


output.write(u"//" + (u"-" * 60) + u"\n");
output.write(u"//          Methods extracted for %s\n" % title.text);
output.write(u"//" + (u"-" * 60) + u"\n");
output.write(u"var methods = " + json.dumps(methodDescriptions, sort_keys=True, indent=2));
output.write(u"\n\n");
