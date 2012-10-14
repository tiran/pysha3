import sys
import unittest
import sha3
import hashlib
import hmac

if sys.version_info[0] == 3:
    fromhex = bytes.fromhex
    tobyte = lambda b: bytes([b])
    asunicode = lambda s: s
else:
    fromhex = lambda s: s.decode("hex")
    tobyte = lambda b: bytes(b)
    asunicode = lambda s: s.decode("ascii")


class BaseSHA3Tests(unittest.TestCase):
    new = None
    name = None
    digest_size = None
    block_size = NotImplemented
    vectors = []

    def assertHashDigest(self, hexmsg, hexdigest):
        hexdigest = hexdigest.lower()
        msg = fromhex(hexmsg)
        digest = fromhex(hexdigest)
        self.assertEqual(len(digest), self.digest_size)

        sha3 = self.new(msg)
        self.assertEqual(sha3.hexdigest(), hexdigest)
        self.assertEqual(sha3.digest(), digest)

        sha3 = self.new()
        sha3.update(msg)
        self.assertEqual(sha3.hexdigest(), hexdigest)
        self.assertEqual(sha3.digest(), digest)

        sha3 = self.new()
        for b in msg:
            sha3.update(tobyte(b))
        self.assertEqual(sha3.hexdigest(), hexdigest)
        self.assertEqual(sha3.digest(), digest)

    def test_basics(self):
        sha3 = self.new()
        self.assertEqual(sha3.name, self.name)
        self.assertEqual(sha3.digest_size, self.digest_size)
        self.assertEqual(sha3.block_size, self.block_size)
        self.assertEqual(len(sha3.digest()), self.digest_size)
        self.assertEqual(len(sha3.hexdigest()), self.digest_size * 2)

        self.assertRaises(AttributeError, setattr, sha3, "digest", 3)
        self.assertRaises(AttributeError, setattr, sha3, "name", "egg")

        self.new(b"data")
        self.new(string=b"data")
        self.assertRaises(TypeError, self.new, None)
        self.assertRaises(TypeError, sha3.update, None)
        self.assertRaises(TypeError, self.new, asunicode("text"))
        self.assertRaises(TypeError, sha3.update, asunicode("text"))

        sha3type = type(sha3)
        self.assertEqual(sha3type.__name__, "SHA3")
        self.assertEqual(sha3type.__module__, "_sha3")
        self.assertRaises(TypeError, sha3type)
        self.assertRaises(TypeError, type, sha3type, "subclass", {})

    def test_hashlib(self):
        constructor = getattr(hashlib, self.name)
        s1 = constructor()
        self.assertEqual(s1.name, self.name)
        self.assertEqual(s1.digest_size, self.digest_size)

        s2 = hashlib.new(self.name)
        self.assertEqual(s2.name, self.name)
        self.assertEqual(s2.digest_size, self.digest_size)
        self.assertEqual(type(s1), type(s2))

        if sys.version_info < (3, 4):
            self.assertEqual(constructor, self.new)

    def test_vectors(self):
        for hexmsg, hexdigest in self.vectors:
            self.assertHashDigest(hexmsg, hexdigest)

    def test_hmac(self):
        self.assertRaises(TypeError, hmac.new, b"", b"", self.new)


class SHA3_224Tests(BaseSHA3Tests):
    new = sha3.sha3_224
    name = "sha3_224"
    digest_size = 28
    vectors = [
        ("", "F71837502BA8E10837BDD8D365ADB85591895602FC552B48B7390ABD"),
        ("CC", "A9CAB59EB40A10B246290F2D6086E32E3689FAF1D26B470C899F2802"),
        ("41FB", "615BA367AFDC35AAC397BC7EB5D58D106A734B24986D5D978FEFD62C"),
        ("433C5303131624C0021D868A30825475E8D0BD3052A022180398F4CA4423B98214B6BEAAC21C8807A2C33F8C93BD42B092CC1B06CEDF3224D5ED1EC29784444F22E08A55AA58542B524B02CD3D5D5F6907AFE71C5D7462224A3F9D9E53E7E0846DCBB4CE",
         "62B10F1B6236EBC2DA72957742A8D4E48E213B5F8934604BFD4D2C3A"),
        ]


class SHA3_256Tests(BaseSHA3Tests):
    new = sha3.sha3_256
    name = "sha3_256"
    digest_size = 32
    vectors = [
        ("", "C5D2460186F7233C927E7DB2DCC703C0E500B653CA82273B7BFAD8045D85A470"),
        ("CC", "EEAD6DBFC7340A56CAEDC044696A168870549A6A7F6F56961E84A54BD9970B8A"),
        ("41FB", "A8EACEDA4D47B3281A795AD9E1EA2122B407BAF9AABCB9E18B5717B7873537D2"),
        ("433C5303131624C0021D868A30825475E8D0BD3052A022180398F4CA4423B98214B6BEAAC21C8807A2C33F8C93BD42B092CC1B06CEDF3224D5ED1EC29784444F22E08A55AA58542B524B02CD3D5D5F6907AFE71C5D7462224A3F9D9E53E7E0846DCBB4CE",
         "CE87A5173BFFD92399221658F801D45C294D9006EE9F3F9D419C8D427748DC41"),
        ]


class SHA3_384Tests(BaseSHA3Tests):
    new = sha3.sha3_384
    name = "sha3_384"
    digest_size = 48
    vectors = [
        ("", "2C23146A63A29ACF99E73B88F8C24EAA7DC60AA771780CCC006AFBFA8FE2479B2DD2B21362337441AC12B515911957FF"),
        ("CC", "1B84E62A46E5A201861754AF5DC95C4A1A69CAF4A796AE405680161E29572641F5FA1E8641D7958336EE7B11C58F73E9"),
        ("41FB", "495CCE2714CD72C8C53C3363D22C58B55960FE26BE0BF3BBC7A3316DD563AD1DB8410E75EEFEA655E39D4670EC0B1792"),
        ("433C5303131624C0021D868A30825475E8D0BD3052A022180398F4CA4423B98214B6BEAAC21C8807A2C33F8C93BD42B092CC1B06CEDF3224D5ED1EC29784444F22E08A55AA58542B524B02CD3D5D5F6907AFE71C5D7462224A3F9D9E53E7E0846DCBB4CE",
         "135114508DD63E279E709C26F7817C0482766CDE49132E3EDF2EEDD8996F4E3596D184100B384868249F1D8B8FDAA2C9"),
        ]


class SHA3_512Tests(BaseSHA3Tests):
    new = sha3.sha3_512
    name = "sha3_512"
    digest_size = 64
    vectors = [
        ("", "0EAB42DE4C3CEB9235FC91ACFFE746B29C29A8C366B7C60E4E67C466F36A4304C00FA9CAF9D87976BA469BCBE06713B435F091EF2769FB160CDAB33D3670680E"),
        ("CC", "8630C13CBD066EA74BBE7FE468FEC1DEE10EDC1254FB4C1B7C5FD69B646E44160B8CE01D05A0908CA790DFB080F4B513BC3B6225ECE7A810371441A5AC666EB9"),
        ("41FB", "551DA6236F8B96FCE9F97F1190E901324F0B45E06DBBB5CDB8355D6ED1DC34B3F0EAE7DCB68622FF232FA3CECE0D4616CDEB3931F93803662A28DF1CD535B731"),
        ("433C5303131624C0021D868A30825475E8D0BD3052A022180398F4CA4423B98214B6BEAAC21C8807A2C33F8C93BD42B092CC1B06CEDF3224D5ED1EC29784444F22E08A55AA58542B524B02CD3D5D5F6907AFE71C5D7462224A3F9D9E53E7E0846DCBB4CE",
         "527D28E341E6B14F4684ADB4B824C496C6482E51149565D3D17226828884306B51D6148A72622C2B75F5D3510B799D8BDC03EAEDE453676A6EC8FE03A1AD0EAB"),
        ]


def test_main():
    suite = unittest.TestSuite()
    classes = [SHA3_224Tests, SHA3_256Tests, SHA3_384Tests, SHA3_512Tests]
    for cls in classes:
        suite.addTests(unittest.makeSuite(cls))
    return suite

if __name__ == "__main__":
    result = unittest.TextTestRunner(verbosity=2).run(test_main())
    sys.exit(not result.wasSuccessful())
