set -e

mongo <<EOF
db = db.getSiblingDB('sa-accomodation-service-mongo')
db.createCollection('accomodations')
db.createCollection('orders')
db.orders.createIndex({"id": -1}) 
db.orders.createIndex({"user_id": -1}) 
EOF