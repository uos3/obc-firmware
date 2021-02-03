'''
Factorisation test.

The Timer module must find the combination of prescalar and timer value that
creates a given duration. This breaks down to the following function:

    t*f_c = p*v

Where t is the duration in seconds, f_c the frequency of the clock, p the
prescalar value and v the timer value. As both p and v are unknown we must find
integer factors of t*f_c.

There are two methods compared here, one based on Pollard's Rho algorithm,
which can find an integer factor of a given number. The other is a naive method
which iterates through prescalar values trying to find an integer factor.

The test will output the average number of iterations, average error, and total
time taken to calculate factors for a number of test cases. Overall it appears
that the naive method is the quickest, even though it produces slightly less
accurate results. Both methods will accept an error of 1 cycle in the
factorisation.
'''

import statistics
import time

def pollard(n):
    def g(x):
        return (x**2 + 1) % n

    x = 2
    y = 2
    d = 1
    i = 0

    while d == 1:
        x = g(x)
        y = g(g(y))
        d = gcd(abs(x - y), n)
        i += 1

    if d == n:
        return (i, None)
    else:
        return (i, d)

def gcd(a, b):
    while b != 0:
        temp = a % b

        a = b
        b = temp

    return a

def least_error_factors(n, v_max):
    min_error_p = 0
    min_error_v = 0
    min_error = n**2

    min_p = max(int(round(n/v_max)), 1)
    i = 0

    for p in range(min_p, 257):
        v = int(round(n/p))
        error = (p*v - n)**2
        if error < min_error:
            min_error = error
            min_error_p = p
            min_error_v = v

            if min_error < 1:
                break

        i += 1

    return (i, min_error_p, min_error_v, min_error)

def get_pv_pollard(n, v_max):
    # First run pollard, if returns none use backup
    (i, a) = pollard(n)
    iters = 0
    b = 0
    e = 0
    
    if (a is None) or (a > 256 and (n/a > 256)) or (a > v_max or (n/a > 256)):
        (j, a, b, e) = least_error_factors(n, v_max)
        iters = i + j
    else:
        b = int(n/a)
        e = (a*b - n)**2
    
    return (iters, a, b, e)

def get_pv_naive(n, v_max):
    # Minimum possible prescale based on v_max
    min_p = max(int(round(n/v_max)) + 1, 1)
    iters = 0
    
    # If min_p would give exact value
    if n % min_p == 0 and min_p != 1:
        v = int(round(n / min_p))
        return (iters, int(n/v), v, (min_p*v - n)**2)
    # If not iterate until we find either error less than 1, or min error
    else:
        min_error_p = 0
        min_error_v = 0
        min_error = n**2

        for p in range(min_p, 257):
            v = int(round(n/p))
            error = (p*v - n)**2
            if error < min_error:
                min_error = error
                min_error_p = p
                min_error_v = v

            # Break if error is less than 1, this is the best we could get
            if min_error < 1:
                break

            iters += 1
        
        return (iters, min_error_p, min_error_v, min_error)

def main():

    naive_method = []
    pollard_method = []

    test_range = range(2**16 - 10000, 2**16 + 10000)
    max_v = 2**16 - 1

    print('Runing Naive')
    start = time.perf_counter()
    for x in test_range:
        naive_method.append(get_pv_naive(x, max_v))
        # Check result is correct (to within 1 cycle)
        assert (naive_method[-1][1]*naive_method[-1][2] - x)**2 <= 1
    naive_time = time.perf_counter() - start

    print('Running Pollard')
    start = time.perf_counter()
    for x in test_range:
        pollard_method.append(get_pv_pollard(x, max_v))
        # Check result is correct (to within 1 cycle)
        assert (pollard_method[-1][1]*pollard_method[-1][2] - x)**2 <= 1
    pollard_time = time.perf_counter() - start
    
    naive_iter_avg = statistics.mean([a[0] for a in naive_method])
    naive_error_avg = statistics.mean([a[3] for a in naive_method])

    pollard_iter_avg = statistics.mean([a[0] for a in pollard_method])
    pollard_error_avg = statistics.mean([a[3] for a in pollard_method])

    print('Results (avg. iters, avg. error, time (s)):')
    print(f'    Naive: {naive_iter_avg}, {naive_error_avg}, {naive_time}')
    print(f'  Pollard: {pollard_iter_avg}, {pollard_error_avg}, {pollard_time}')

if __name__ == '__main__':
    main()