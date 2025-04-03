clear
close all

%% В этом скрипте пробую построить тепловую карту по 
% изображению спирали нагревателя


%% Строю бинарное изображение спирали нагревателя
%% по исходной картинке
A=imread('test-1.bmp');
figure,imshow(A);title('Загруженное изображение');
A=rgb2gray(A);
A=im2double(A);
L=graythresh(A);
B=im2bw(A,L);
figure,imshow(B);title('Бинаризованное изображение');

B=imdilate(B,ones(3));
B=medfilt2(B); % remove corners
B=imcomplement(B);

figure,imshow(B);title('После фильтрации шума')
%% Имитация нагрева при помощи гауссова размытия

m=fspecial('gaussian',50,10);

C=imfilter(im2double(B),m);
figure,imshow(C,[]); title('Модель распространения тепла')

figure,imshow(C+B*0.25,[]);
title('Модель распространения тепла плюс спираль, k=0.25');
figure,imshow(C+B*0.05,[]);
title('Модель распространения тепла плюс спираль, k=0.05 (лучше)');


%% Пробую разные режимы распространения тепла по воздуху
Sigma=[10 15 20 25 30];
for pass=Sigma
    m=fspecial('gaussian',100,pass);
    C=imfilter(im2double(B),m);
    figure,imshow(C+B*0.05,[]);
    title("Эксперимент: Sigma="+pass);
end    


%% Здесь два процесса распространения:
% с малой сигмой - по нагревателю,
% с большой - по воздуху.
% Потом всё это складываю.

m1=fspecial('gaussian',50,5);
C1=imfilter(im2double(B),m1); % Спираль

m2=fspecial('gaussian',100,25);
C2=imfilter(im2double(B),m2); % Воздух

C3=C1*0.25+C2;
C3=imadjust(C3);
figure,imshow(C3,[]);title('Double gauss');

C=C3;
figure,imshow(Violet(C3));
title('Модель Double Gauss В фиолетовых тонах')

%% То же самое, но расчеты вынесены в функцию
H=GenerateHeatMap(B,5,25,0.25,1);
H=imadjust(H);
figure,imshow(Violet(H));
title('Модель Double Gauss (одной функцией)')
% imwrite(H,'Heat1.bmp');  % Полученное изображение можно сохранить

%% Тут разбираюсь, как перевести полутоновую картинку 
%  в фиолетовые тона
% (писал на коленке, поэтому работает немного криво)

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

figure,imshow(D,[]);title('Проверка перевода в фиолетовую схему')

%% Вынес перевод в фиолетовую схему в отдельную функцию
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

%% Вынес схему с двойным Гауссом в отдельную функцию
function H=GenerateHeatMap(B,s1,s2,k1,k2)
if isempty(k2),k2=1;end
m1=fspecial('gaussian',50,s1);
C1=imfilter(im2double(B),m1);

m2=fspecial('gaussian',100,s2);
C2=imfilter(im2double(B),m2);

H=C1*k1+C2*k2;

end