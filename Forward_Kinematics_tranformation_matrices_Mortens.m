syms theta1
syms theta2
syms theta3
A = [1 0 0 0; 
     0 1 0 0;
     0 0 1 0.245;
     0 0 0 1];

Z = [1 0 0 0.09; 
     0 1 0 0;
     0 0 1 0;
     0 0 0 1]; 
 
C = [cos(theta1) -sin(theta1) 0 0;
     sin(theta1)*cos(0) cos(theta1)*cos(0) -sin(0) -sin(0)*0;
     sin(theta1)*sin(0) cos(theta1)*sin(0) cos(0) cos(0)*0;
     0 0 0 1];
%-90
D = [cos(theta2+90) -sin(theta2+90) 0 0;
     sin(theta2+90)*cos(+90) cos(theta2+90)*cos(+90) -sin(+90) -sin(+90)*0;
     sin(theta2+90)*sin(+90) cos(theta2+90)*sin(+90) cos(+90) cos(+90)*0;
     0 0 0 1];

%  D = [cos(theta2+deg2rad(-90)) -sin(theta2+deg2rad(-90)) 0 0;
%      sin(theta2+deg2rad(-90))*cos(deg2rad(-90)) cos(theta2+deg2rad(-90))*cos(deg2rad(-90)) -sin(deg2rad(-90)) -sin(deg2rad(-90))*0;
%      sin(theta2+deg2rad(-90))*sin(deg2rad(-90)) cos(theta2+deg2rad(-90))*sin(deg2rad(-90)) cos(deg2rad(-90)) cos(deg2rad(-90))*0;
%      0 0 0 1]
 
E = [cos(theta3) -sin(theta3) 0 0.22;
     sin(theta3)*cos(0) cos(theta3)*cos(0) -sin(0) -sin(0)*0;
     sin(theta3)*sin(0) cos(theta3)*sin(0) cos(0) cos(0)*0;
     0 0 0 1];
 
F = [1 0 0 0.175;
     0 1 0 0;
     0 0 1 0;
     0 0 0 1];
 
 MultiCDEF = C*D*E*F;
 
 FinalMulti = A*MultiCDEF*Z
 
% FinalMulti2 = A*C*D*E*F*Z