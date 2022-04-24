data = importdata('sensors-data-left-side.txt', '\t');
l = data(:,1);
v = data(:,2);

F = @(a, data) a(3) + exp(a(1)./(data + a(2)));
x0 = [2585 270 135];

fit = lsqcurvefit(F, x0, l, v)

plot(l, F(fit, l), l, v, 'ro')