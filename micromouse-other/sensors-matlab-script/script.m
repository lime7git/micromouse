data = importdata('sensors-data.txt', '\t');
l = data(:,1);
v = data(:,2);

F = @(a, data) a(3) + exp(a(1)./(data + a(2)));
x0 = [2350 205 125];

fit = lsqcurvefit(F, x0, l, v)

plot(l, F(fit, l), l, v, 'ro')