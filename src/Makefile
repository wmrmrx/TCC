BOOST_DIR = boost_1_85_0
BOOST_URL = https://boostorg.jfrog.io/artifactory/main/release/1.85.0/source/boost_1_85_0.tar.gz

artifacts/boost.tar.gz:
	wget $(BOOST_URL) -O artifacts/boost.tar.gz

include/boost: artifacts/boost.tar.gz
	tar xf artifacts/boost.tar.gz -C include
	mv include/$(BOOST_DIR) include/boost
