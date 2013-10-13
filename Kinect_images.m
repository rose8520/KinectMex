%%Obtain images from kinect
% Get RGB and DEPTH image via Kinect

close all; clear all;
addpath('./Mex');
%% Create context with xml file
context = mxNiCreateContext('Config/SamplesConfig.xml');

%% Initialise FIGURE
width = 640; height = 480;
% depth image
figure, h1 = imagesc(zeros(height,width,'uint16'));
% rgb image
figure, h2 = imagesc(zeros(height,width,3,'uint8'));
%  rgb+depth image
figure, h3 = imagesc(zeros(height,width,3,'uint8')); hold on;
        h4 = imagesc(zeros(height,width,'uint16'));  hold off;

%% LOOP
for k=1:5 
    tic
    %Align Depth onto RGB
    option.adjust_view_point = true;
    
    % Acquire RGB and Depth image
    mxNiUpdateContext(context, option);
    [rgb, depth] = mxNiImage(context);
   
    % Save image
    imwrite(rgb,strcat('demoday',int2str(k),'.jpg'),'jpg');
    imwrite(depth,strcat('demodaydepth',int2str(k),'.jpg'),'jpg');
    
    dance(:,:,:,k)=rgb;
    
    % Update figure 
    set(h1,'CData',depth); 
    set(h2,'CData',rgb); 
    set(h3,'CData',rgb); 
    set(h4,'CData',depth);
    set(h4,'AlphaData',double(depth/50));
    drawnow;
    disp(['itr=' sprintf('%d',k) , ' : FPS=' sprintf('%f',1/toc)]);
    
end

%% Delete the context object
mxNiDeleteContext(context);