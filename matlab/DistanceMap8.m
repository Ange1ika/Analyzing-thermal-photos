function [ B ] = DistanceMap8( A )
%DistanceMap Считает карту расстояний в 8-соседстве
%Расстояния идут с шагом 10 (1 пиксель = 10 ед)

[w,h]=size(A);
B=zeros(size(A));

for x=2:w-1
    for y=2:h-1
        if A(x,y)==0, continue; end;
        L=min([B(x-1,y)+10,B(x,y-1)+10,B(x-1,y-1)+14,B(x-1,y+1)+14]);
        B(x,y)=L;
    end
end
   
for x=w-1:-1:2
    for y=h-1:-1:2
        if B(x,y)==0, continue;end;
        L=min([B(x+1,y)+10,B(x,y+1)+10,B(x+1,y+1)+14, B(x+1,y-1)+14,B(x,y)]);
        B(x,y)=L;
    end
end


end

