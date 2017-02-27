clear; close all; clc

if ~libisloaded('libMARTA') && ~exist('pMARTA', 'file');
    [~,~] = loadlibrary('libMARTA.dll', 'marta.h', 'mfilename', 'pMARTA');
end

if libisloaded('libMARTA')
    unloadlibrary('libMARTA');
end