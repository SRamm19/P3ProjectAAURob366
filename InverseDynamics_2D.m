  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%    the inverse dynamics of the 2-dof planar robotic arm
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

clear all; close all; clc

%%  %%%%%%%%%%%%%%%%%%%%%%%  motion profiles
%=======================  joint 1
A1 = 0; % magnitude
f1 = pi; % frequency

%=======================  joint 2
A2 = 0; % magnitude
f2 = 3*pi; % frequency

g = 9.801; % gravity constant

%% %%%%%%%%%%%%%%%%%%%%%%%%%%%%  link properties
%Unit is in name(kg, M und so weiter)
MX_28kg = 0.083; %kg
MX_64kg = 0.126; %kg
MX_106kg = 0.153; %kg
Gir4derholeskg = 0.0368543801; %kg  
Gir4derholesM = 0.127; %meter
Gir2derholeskg = 0.0226796185; %kg
Gir2derholesM = 0.0635; % meter
SingleAxisAdapterkg = 0.0170097139; %kg
Gripperkg = 0.058; %kg
GripperM = 0.125; %meter
%======================================  link 1
L1 = 0.22; % length [m]
c1 = ((0.041*0.0278/2)+(Gir4derholeskg*(0.0278+(0.127/2)))+(MX_64kg*(0.0278+0.127+(0.0611/2))))/(0.041+Gir4derholeskg+MX_64kg); % mass center
m1 = MX_64kg+Gir4derholeskg+0.041; % mass [kg]
I1 = 1/12*m1*L1^2; % moment of inertia - WIP
%======================================  link 2
L2 = 0.265; % length [m]
c2 = ((0.041*0.0278/2)+(Gir2derholeskg*(0.0278+(0.0635/2)))+((2*MX_28kg)*(0.0278+0.0635+(0.05/2)))+(Gripperkg*(0.0278+0.0635+0.05+(0.125/2))))/(0.041+Gir2derholeskg+(2*MX_28kg)+Gripperkg); % mass center
m2 = Gripperkg+Gir2derholeskg+0.041+2*MX_28kg; % mass [kg] - 0.041 from the attachmentthingy on the motor
I2 = 1/12*m2*L2^2; % moment of inertia - WIP

%% %%%%%%%%%%%%%%%%%% dynamic simulation
%%%%%%%%%%%%%%%%%%% discrete time
T = 1; % second
N = 100; % resolution
i = 0; 
for t = linspace(0, T, N)
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%% instantaneous time
    i = i + 1; time(i) = t; 
    
    %%%%%%%%%%%%%%% Joint 1: angular displacement, velocity, acceleration
    theta_1(i) = A1*sin(f1*t); dtheta_1(i) = A1*f1*cos(f1*t); 
    ddtheta_1(i) = -A1*f1^2*sin(f1*t); 
    
    %%%%%%%%%%%%%%% Joint 1: angular displacement, velocity, acceleration
    theta_2(i) = A2*sin(f2*t); dtheta_2(i) = A2*f2*cos(f2*t);
    ddtheta_2(i) = -A2*f2^2*sin(f2*t); 
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%  coefficients of dynamic equation
    H11 = m1*c1^2 + I1 + m2*(L1^2 + c2^2 + 2*L1*c2*cos(theta_2(i))) + I2;
    H22 = m2*c2^2 + I2; 
    H12 = m2*(c2^2 + L1*c2*cos(theta_2(i))) + I2; 
    h = m2*L1*c2*sin(theta_2(i)); 
    G1 = m1*c1*g*cos(theta_1(i)) ...
        + m2*g*(c2*cos(theta_1(i) + theta_2(i)) + L1*cos(theta_1(i))); 
    G2 = m2*g*c2*cos(theta_1(i) + theta_2(i));
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  actuator torques
    tau_1(i) = H11*ddtheta_1(i) + H12*ddtheta_2(i) - h*(dtheta_2(i))^2 ...
        -2*h*dtheta_1(i)*dtheta_2(i) + G1;
    tau_2(i) = H22*ddtheta_2(i) + H12*ddtheta_1(i) + h*(dtheta_1(i))^2 + G2;
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  link positions
    %%%%%%%%%%%%%%%%%% link 1
    X1(i) = L1*cos(theta_1(i)); Y1(i) = L1*sin(theta_1(i)); 
    x1(i) = 0.5*X1(i); y1(i) = 0.5*Y1(i); 
    %%%%%%%%%%%%%%%%%% link 2
    X2(i) = X1(i) + L2*cos(theta_1(i) + theta_2(i)); 
    Y2(i) = Y1(i) + L2*sin(theta_1(i) + theta_2(i)); 
    x2(i) = X1(i) + 0.5*X2(i); Y2(i) = Y1(i) + 0.5*Y2(i); 
    
end

torque_1 = [time; tau_1]'; torque_2 = [time; tau_2]'; 

%% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% plot the motion profiles
%figure(2)
%clf
%figure(2)
%subplot(3, 1, 1)
%hold on
%plot(time, theta_1, 'b')
%plot(time, theta_2, 'r')
%hold off
%legend('joint 1', 'joint 2')
%grid on; 
%xlabel('time [sec]'); ylabel('angular displacement [rad]'); 
%subplot(3, 1, 2)
%hold on
%plot(time, dtheta_1, 'b')
%plot(time, dtheta_2, 'r')
%hold off
%grid on; 
%xlabel('time [sec]'); ylabel('angular velocity [rad/s]'); 
%subplot(3, 1, 3)
%hold on
%plot(time, ddtheta_1, 'b')
%plot(time, ddtheta_2, 'r')
%hold off
%grid on; 
%xlabel('time [sec]'); ylabel('angular acceleration [rad/s^2]'); 

%% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% plot the motor torques
figure(1)
clf
figure(1)
hold on
plot(time, tau_1, 'b')
plot(time, tau_2, 'r')
hold off
legend(' joint 1', 'joint 2')
grid on; 
xlabel('time [sec]'); ylabel('torques [Nm/rad]'); 

disp(tau_1) 
%% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  plot the link
figure(2)
clf
figure(2)
hold on 
axis equal; grid on; axis([-1 1 -1 1]); 
xlabel('x [m]'); ylabel('z [m]'); 
set(gca,'NextPlot','replaceChildren');
for j = 1 : N  
    plot(0, 0, 'ko', 'MarkerSize', 6, 'Linewidth', 2)  
    plot(X1(j), Y1(j), 'bo', 'MarkerSize', 6, 'Linewidth', 2)
    plot(X2(j), Y2(j), 'ro', 'MarkerSize', 6, 'Linewidth', 2)
    line([0 X1(j)], [0 Y1(j)], 'Color', 'b', 'Linewidth', 2); 
    line([X1(j) X2(j)], [Y1(j) Y2(j)], 'Color', 'r', 'Linewidth', 2);  
    F(j) = getframe; 
end
hold off
movie(F); 