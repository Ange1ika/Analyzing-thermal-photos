clear
close all

%% В этом скрипте пробую сделать бинаризацию тепловой карты,
% чтобы получить изображение спирали нагревателя

H=imread('Heat1.bmp');
figure,imshow(H);

%1. Пробую найти точки границы
A=im2gray(H);
A=im2double(A);
E=edge(A,'sobel');
figure,imshow(E);title('Границы');
% Границы, конечно, плохонькие, но какие уж есть.

% 2.Если границы найдены - их можно использовать для расчета порога
% бинаризации

E1=E;
E1=imdilate(E,ones(3));
[y,x]=find(E1==1);
[h,w]=size(E);

es=0;ns=0;

for i=1:length(x)
    es=es+A(y(i),x(i));
    ns=ns+1;
end

es=es/ns;

D=zeros(h,w);
D(find(A>es))=1;
figure,imshow(D);  
title('Восстановленное изображение нагревателя');

%% Строим карту расстояний
M=DistanceMap8(D);
figure,imshow(M,[]);
title('Карта расстояний до границ')

% Находим локальные максимумы карты.
% Они образуют каркас (скелет) нагревателя
[h,w]=size(D);
S=zeros(size(D));
R=S;
for x=2:w-1
    for y=2:h-1
        if (M(y,x)>0)...
        && (M(y,x)>=M(y,x+1))...
        && (M(y,x)>=M(y,x-1))...
        && (M(y,x)>=M(y+1,x))...
        && (M(y,x)>=M(y-1,x))        
        S(y,x)=1;
        R(y,x)=M(y,x);
        end
    end
end

figure,imshow(S,[]);title('Скелетная линия');
figure,imshow(R,[]);title('Радиус');

% Отклонения радиуса R от среднего, по идее, даст точки дефектов.
% Отклонения скелетной линии от образца даст точки КЗ

%%----------------------------------------------------------
function D=Violet(C)
RT=zeros(256,1);GT=RT;BT=RT;
BT(1:128)=0:2:255;
BT(129:end)=255;

GT(1:end)=0:255;
RT(1:64)=0:2:126;
RT(65:128)=126:-2:0;
RT(193:end)=0:4:254;

[h,w]=size(C);
D=zeros(h,w,3);

for x=1:w
    for y=1:h
        cc=round(C(y,x)*256+1);
        if cc<1,cc=1;end
        if cc>256,cc=256;end

        D(y,x,3)=RT(cc)/255;
        D(y,x,2)=GT(cc)/255;
        D(y,x,1)=BT(cc)/255;        
    end
end
end


function H=GenerateHeatMap(B,s1,s2,k1,k2)
if isempty(k2),k2=1;end
m1=fspecial('gaussian',50,s1);
C1=imfilter(im2double(B),m1);

m2=fspecial('gaussian',100,s2);
C2=imfilter(im2double(B),m2);

H=C1*k1+C2*k2;

end