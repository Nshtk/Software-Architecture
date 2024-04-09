db = db.getSiblingDB('sa-accommodation-service-mongo')
db.createCollection('accommodations')
db.createCollection('orders')
db.orders.createIndex({"id": -1}) 
db.orders.createIndex({"user_id": -1}) 
db.accommodations.insertMany([
 {
	_id: 1,
    name: 'Sample Accommodation 1',
    description: 'Sample accommodation description 1',
    price: '1',
	deadline: new Date('2024-04-01T12:00:00Z')
  },
  {
	_id: 2,
    name: 'Sample Accommodation 2',
    description: 'Sample accommodation description 2',
    price: '2',
	deadline: new Date('2024-04-02T13:00:00Z')
  },
  {
	_id: 3,
    name: 'Sample Accommodation 3',
    description: 'Sample accommodation description 3',
    price: '3',
	deadline: new Date('2024-04-03T14:00:00Z')
  },
]);
db.orders.insertMany([
  {
	_id: 1,
	user_id: 1,
	accommodations: [
		{accommodation_id: 1, accommodation_name: 'Sample accommodation 1'},
		{accommodation_id: 2, accommodation_name: 'Sample accommodation 2'}
	]
  },
  {
	_id: 2,
	user_id: 2,
	accommodations: [
		{accommodation_id: 1, accommodation_name: 'Sample accommodation 1'}
	]
  },
  {
	_id: 3,
	user_id: 3,
	accommodations: [
		{accommodation_id: 2, accommodation_name: 'Sample accommodation 2'},
		{accommodation_id: 3, accommodation_name: 'Sample accommodation 3'}
	]
  },
  {
	_id: 4,
	user_id: 4,
	accommodations: [
		{accommodation_id: 3, accommodation_name: 'Sample accommodation 3'},
		{accommodation_id: 1, accommodation_name: 'Sample accommodation 1'}
	]
  },
  {
	_id: 5,
	user_id: 5,
	accommodations: [
		{accommodation_id: 3, accommodation_name: 'Sample accommodation 3'}
	]
  },
]);
