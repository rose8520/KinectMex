clear all; close all; clc

OpenNiPath='C:\Program Files\OpenNI\';
OpenNiPathLib=[OpenNiPath 'Lib64'];
OpenNiPathInclude=[OpenNiPath 'Include'];

files=dir('*.cpp');

for i=1:length(files)
    Filename=files(i).name;
    clear(Filename); 
    mex('-v',['-L' OpenNiPathLib], '-lopenNI64', ['-I' OpenNiPathInclude], Filename);
end
