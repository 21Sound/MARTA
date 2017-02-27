clear; close all; clc

if ~libisloaded('libMARTA') && ~exist('pMARTA', 'file');
    [~,~] = loadlibrary('libMARTA.dll', 'MARTA.h', 'mfilename', 'pMARTA');
end

if libisloaded('libMARTA')
    unloadlibrary('libMARTA');
end