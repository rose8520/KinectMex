%% Load the images

%load allinone.mat


%% Make 20 images from the 100 for camera calibration

selectrand=randperm(100); 
select20=sort(selectrand(1:20))

for ii=1:20
    
    imagebuffer = uint8(rgbs(:,end:-1:1,:, select20(ii)));
    imwrite(imagebuffer, strcat('myimage',num2str(ii),'.jpg'), 'jpeg');
end
    
%% Run Calibraiton

calib
