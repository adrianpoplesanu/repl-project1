class Dog:
    def test(self):
        self.test2(a=5, b=7)

    def test2(self, a, b):
        print(a + b)


dex = Dog()
dex.test()
