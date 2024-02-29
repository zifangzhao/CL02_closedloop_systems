function coeffs = CE32_filterDesigner(cutoffFreq, samplingFreq, order)
    coeffs = struct('a0', [], 'a1', [], 'a2', [], 'b0', [], 'b1', [], 'b2', []);
    wc = tan(pi * cutoffFreq / samplingFreq);
    wc2 = wc^2;
    sqrt2wc = sqrt(2) * wc;

    a = 1;
    b = 1;

    for i = 0:(order/2 - 1)
        theta = pi * (2 * i + 1) / (2 * order);
        alpha = sin(theta) / (2 * wc);
        beta = 1 + alpha;

        coeffs(i+1).b0 = alpha / beta;
        coeffs(i+1).b1 = 0;
        coeffs(i+1).b2 = -alpha / beta;
        coeffs(i+1).a0 = 1;
        coeffs(i+1).a1 = -2 * cos(theta) / beta;
        coeffs(i+1).a2 = (1 - alpha) / beta;

        a = a * coeffs(i+1).a2;
        b = b * coeffs(i+1).b2;
    end

    if mod(order, 2) ~= 0
        alpha = 1 / wc2;
        beta = 1 + sqrt2wc + wc2;

        coeffs(order/2 + 1).b0 = alpha / beta;
        coeffs(order/2 + 1).b1 = 0;
        coeffs(order/2 + 1).b2 = -alpha / beta;
        coeffs(order/2 + 1).a0 = 1;
        coeffs(order/2 + 1).a1 = -2 * (wc2 - 1) / beta;
        coeffs(order/2 + 1).a2 = (1 - sqrt2wc + wc2) / beta;
    end

    for i = 1:(order/2)
        coeffs(i).a0 = coeffs(i).a0 / a;
        coeffs(i).a1 = coeffs(i).a1 / a;
        coeffs(i).a2 = coeffs(i).a2 / a;
        coeffs(i).b0 = coeffs(i).b0 / b;
        coeffs(i).b1 = coeffs(i).b1 / b;
        coeffs(i).b2 = coeffs(i).b2 / b;
    end

    if mod(order, 2) ~= 0
        coeffs(order/2 + 1).a0 = coeffs(order/2 + 1).a0 / a;
        coeffs(order/2 + 1).a1 = coeffs(order/2 + 1).a1 / a;
        coeffs(order/2 + 1).a2 = coeffs(order/2 + 1).a2 / a;
        coeffs(order/2 + 1).b0 = coeffs(order/2 + 1).b0 / b;
        coeffs(order/2 + 1).b1 = coeffs(order/2 + 1).b1 / b;
        coeffs(order/2 + 1).b2 = coeffs(order/2 + 1).b2 / b;
    end
end
