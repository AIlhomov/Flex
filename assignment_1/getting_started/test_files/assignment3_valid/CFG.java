public class CFG {
    public static void main(String[] a) {
        System.out.println(new Sum().calcSum(100));
    }
}

class Sum {
    public int calcSum(int num) {
        int sum;
        sum = 0;
        while (0 < num) {
            while (0 < num) {
                while (0 < num) {
                    if (50 < num && sum < 2000) {
                        System.out.println(sum);
                    } else {
                        System.out.println(num);
                    }
                    sum = sum + num;
                    num = num - 1;
                }
            }
        }
        return sum;
    }

    public int calcSum1(int num) {
        int sum;
        sum = 0;
        while (0 < num) {
            while (0 < num) {
                while (0 < num) {
                    while (0 < num) {
                        sum = sum + num;
                        num = num - 1;
                    }
                }
            }
        }
        return sum;
    }

    public int calcSum2(int num) {
        int sum;
        sum = 0;
        if (50 < num && sum < 2000) {
            if (50 < num && sum < 2000) {
                if (50 < num && sum < 2000) {
                    System.out.println(sum);
                } else {
                    System.out.println(num);
                }
            } else {
                System.out.println(num);
            }
        } else {
            System.out.println(num);
        }
        return sum;
    }
}
