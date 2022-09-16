cd ../../java/ad ;
mvn clean install ;
cp target/ad-1.0-SNAPSHOT.jar . ;
cd ../../integration-tests/java ;
cp -r ../../java/ad/bootstrap . ;
cp -r ../../java/ad/examples .