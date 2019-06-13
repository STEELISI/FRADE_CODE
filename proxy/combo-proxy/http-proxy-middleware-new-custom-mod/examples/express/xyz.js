var express = require('express')
var fs = require('fs')
var morgan = require('morgan')
var path = require('path')
var x ="10.2.1.5"
if(!(x.indexOf("10.1") >= 0))
{
  console.log("found");
  console.log(x);
}
else
{
  console.log("Not found");
  console.log(x);
}
